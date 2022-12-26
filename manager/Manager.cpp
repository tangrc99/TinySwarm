//
// Created by 唐仁初 on 2022/7/28.
//

#include "Manager.h"

namespace manager{

    CreateResult
    Manager::selectWorkerToCreatePod(const string &service, const string &alias, ServiceType type, int port,
                                     const std::vector<char *> &exe_params, const std::vector<char *> &docker_params,
                                     int restart) {

        PodDescriptor &pod = pods.emplace(alias,
                                          PodDescriptor(service, alias, type, false, port, nullptr,
                                                        exe_params, docker_params, restart)).first->second;

        // 将 pod 绑定到选定的 worker
        auto wd = selectWorker(&pod);
        pod.bindWorker(wd);

        if (pod.wd_ == nullptr) {
            pods.erase(alias);
            return CreateResult("Available Worker Not Found");
        }

        auto res = createService(pod);

        if (res.isFail())
            pods.erase(alias);

        return res;
    }

    CreateResult
    Manager::createPod(WorkerDescriptor *wd, const string &service, const string &alias, ServiceType type, int port,
                       const std::vector<char *> &exe_params, const std::vector<char *> &docker_params, int restart) {

        if (wd == nullptr)
            return CreateResult("Empty Worker");

        ForkInput input;
        input.set_service(service);
        input.set_alias(alias);
        input.set_type(type);
        for (auto &param: exe_params) {
            input.add_exe_params(param);
        }
        for (auto &param: docker_params) {
            input.add_docker_params(param);
        }
        input.set_restart(restart);
        input.set_owner(usr_info);


        // 提前创建好对应服务的描述量
        PodDescriptor info(service, alias, type, 1, port, wd, exe_params, docker_params, restart);

        auto res = pods.emplace(alias, info);    // 这里优先操作内存，因为容易恢复

        if (!res.second) {
            return CreateResult("services emplace error");
        }

        // log << create service ...
        cur_line = logger->append("CSS " + alias);

        retry :
        auto rpc_res = wd->session->run(FORK, &input);
        // log << create success

        if (rpc_res.isFailed()) {
            cur_line = logger->append("CSE " + alias + " Fail");
            pods.erase(res.first);  // 如果失败则进行回退
            return CreateResult(rpc_res.ErrorText());    //FIXME: 这里的错误要分层级，否则用户端不知道错误是出在哪次 RPC 上
        }

        ForkEcho echo;
        rpc_res.castMessageTo(&echo);

        if (echo.fail()) {
            if (echo.error_text() == "Manager Down") {

                std::cout << "receive manager down" << std::endl;

                for (auto &serv: wd->pods) {
                    serv->alive_ = false;
                    down_services.emplace_back(serv);
                }

                wd->pods = {};  // 将当前 worker 的服务设置为下线

                usleep(100);
                goto retry;     //FIXME: 这里能够正常使用吗？

            } else {
                cur_line = logger->append("CSE " + alias + " Fail");
                pods.erase(res.first);
                return CreateResult(echo.error_text());
            }
        }

        cur_line = logger->append("CSE " + info.toSnapshotMessage());

        wd->pods.emplace_back(&info);

        publishMessage(res.first->second);

        return CreateResult(&res.first->second);
    }

    std::pair<bool, std::string> Manager::stopPod(PodDescriptor *info) {

        auto it = pods.find(info->alias_);
        if (it == pods.end())
            return {false, "Alias Not Found"};

        info->wd_->removePodDescriptor(info); // 删除 worker 上的指针

        auto[rpc_res, error] = shutdownPod(info);

        if (rpc_res) {

            pods.erase(it); // 删除 service 本身信息

            if (!info->alive_) {   // 如果在用户命令前该服务已经停止了，那么需要将其从失活链表中取出
                removeDownService(info);
            }

            return {true, info->alias_};
        }


        info->wd_->pods.emplace_back(info);

        return {false, error};
    }

    std::pair<bool, std::string> Manager::transferPod(PodDescriptor &info, WorkerDescriptor *new_wd) {

        PodDescriptor old_info = info;
        info.bindWorker(new_wd);

        auto res = createService(info);

        if (res.isFail()) {
            std::cerr << res.reason() << std::endl;
            info = old_info;
            return {false, "CREATE FAIL"};
        }

        if (old_info.alive_ == 1) {
            // 如果还存活的话，需要先关闭。
            old_info.wd_->removePodDescriptor(&info);

            auto[res1, error1] = shutdownPod(&old_info);

            if (!res1) {
                std::cerr << error1 << std::endl;    // 这里失败的话，直接将service从manager 去除，一段时间后worker也会去除
            }
        }

        //FIXME: 这里如果失败会导致直接崩溃
        info.wd_->pods.emplace_back(&info);

        return {true, info.alias_};
    }

    void Manager::checkPodsOnAllWorker(int line) {

        auto now = time(nullptr);

        if (now < check_time) {
            return;
        }

        check_time = now + 5;

        for (auto worker: workers) {    // 如果节点存活，则发送检查请求
            if (worker->alive)
                checkServices(worker, line);
            else    // 如果节点失活，则尝试重连节点
                worker->alive = worker->session->reconnect();
        }
    }

    WorkerDescriptor *Manager::connectToWorker(IPAddress ipAddress) {
        auto session = cli.createSessionOnNewChannel(ipAddress);

        auto wd = new WorkerDescriptor;
        wd->ip = ipAddress.getAddressWithoutPort();
        wd->port = std::stoi(ipAddress.getPort());
        wd->session = session;
        wd->alive = session->connected();

        workers.emplace_back(wd);
        return wd;
    }

    WorkerDescriptor *Manager::findWorkerDescriptor(const string &ip, int port) {
        for (auto &wd: workers) {
            if (wd->ip == ip && wd->port == port)
                return wd;
        }
        return nullptr;
    }

    WorkerDescriptor *Manager::findWorkerDescriptor(const string &address) {
        std::string ip, port;
        std::stringstream ss(address);
        std::getline(ss, ip, ':');
        std::getline(ss, port, ':');

        string tmp;
        ss.clear();
        ss.str("");
        ss << ip;

        while (std::getline(ss, tmp, '.')) {
            int a = stoi(tmp);
            if (a < 0 || a > 255)
                return nullptr;
        }

        for (auto &ch: port) {
            if (!isnumber(ch))
                return nullptr;
        }
//FIXME: 这里必须要返回给用户错误信息
        return findWorkerDescriptor(ip, stoi(port));
    }

    PodDescriptor *Manager::getPodDescriptorByAlias(const string &alias) {
        auto it = pods.find(alias);
        return it == pods.end() ? nullptr : &it->second;
    }

    void Manager::removeDownService(PodDescriptor *pod) {
        if (pod == nullptr)
            return;

        for (auto it = down_services.begin(); it != down_services.end();) {
            if (*it == pod) {
                down_services.erase(it);
                return;
            } else {
                it++;
            }
        }
    }

    void Manager::transferDownPod() {

        // 如果没有下线服务，直接返回
        if (down_services.empty())
            return;

        for (auto &pod: down_services) {

            // 如果没有存活，尝试转移到另外一个地方
            if (!pod->wd_->alive) {

                auto wd = selectWorker(pod);
                auto[res, error] = transferPod(*pod, wd);  // 转移 service 到另外一个 worker 上
                if (res) {
                    removeDownService(pod);
                }

            } else {

                auto res = createService(*pod);    // 当前节点如果还存活，则直接对服务进行重启

                if (!res.isFail())
                    removeDownService(pod);
            }
        }
    }

    nlohmann::json Manager::showWorkerNodes() {
        nlohmann::json result;
        std::vector<nlohmann::json> worker_list;
        worker_list.reserve(workers.size());

        for (auto worker: workers)
            worker_list.emplace_back(worker->toJson());

        result["workers"] = worker_list;

        return result;
    }

    CreateResult Manager::createService(PodDescriptor &info) {

        ForkInput input;
        input.set_service(info.service_);
        input.set_alias(info.alias_);
        input.set_type(info.type_);
        for (auto &param: info.exe_params_) {
            input.add_exe_params(param);
        }
        for (auto &param: info.docker_params_) {
            input.add_docker_params(param);
        }
        input.set_restart(info.restart_);
        input.set_owner(usr_info);


        // log << create service ...
        cur_line = logger->append("CSS " + info.alias_);
        retry :
        auto rpc_res = info.wd_->session->run(FORK, &input);

        if (rpc_res.isFailed()) {
            cur_line = logger->append("CSE " + info.alias_ + " Fail");
            return CreateResult(rpc_res.ErrorText());
        }

        ForkEcho echo;
        rpc_res.castMessageTo(&echo);

        if (echo.fail()) {

            if (echo.error_text() == "Manager Down") {

                for (auto &service: info.wd_->pods) {
                    service->alive_ = false;
                    down_services.emplace_back(service);
                }
                info.wd_->pods = {};  // 将当前 worker 的服务设置为下线

                goto retry;     //FIXME: 这里能够正常使用吗？
            } else {
                cur_line = logger->append("CSE " + info.alias_ + " Fail");
                return CreateResult(echo.error_text());
            }
        }
        // log << create success
        cur_line = logger->append("CSE " + info.toSnapshotMessage());

        publishMessage(info);

        info.alive_ = true;

        if (pods.find(info.alias_) == pods.end()) {
            PodDescriptor &ref = pods.emplace(info.alias_, info).first->second;
            return CreateResult(&ref);
        }

        return CreateResult(&info);
    }

    void Manager::checkServices(WorkerDescriptor *wd, int line) {

        if (!wd->alive)
            return;

        CheckInput input;
        input.set_line(line);
        input.set_user_info(usr_info);
        DownServices downServices;
        std::string error_text;

        auto rpc_res = wd->session->run(CHECK, &input);

        if (rpc_res.isFailed()) {
            std::cout << "worker is down" << std::endl;
            wd->alive = false;  // 如果 worker 没有存活，将所有的服务转为下线
            for (auto &service: wd->pods) {
                service->alive_ = false;
                down_services.emplace_back(service);
            }
            return;
        }

        rpc_res.castMessageTo(&downServices);

        // 如果没有服务下线，那么直接返回即可
        if (downServices.service_size() == 0)
            return;

        // 代表 manager 长时间未与 worker 连接，所有的服务都已经被下线
        if (downServices.service(0).alias() == "*") {
            for (auto &service: wd->pods) {
                service->alive_ = false;
                service->error = new PodExitError();
                service->error->exit = downServices.service(0).exit_num();
                service->error->out_file = downServices.service(0).out_file();
                service->error->reason = downServices.service(0).error_text();
            }
            return;
        }


        // 如果是部分节点下线，则收集错误
        for (int i = 0; i < downServices.service_size(); i++) {
            auto alias = downServices.service(i).alias();
            auto info = getPodDescriptorByAlias(alias);
            if (info == nullptr) {
                std::cerr << "No info named alias: " << alias << std::endl;
                continue;
            }
            info->alive_ = false;
            info->error = new PodExitError();
            info->error->exit = downServices.service(i).exit_num();
            info->error->out_file = downServices.service(i).out_file();
            info->error->reason = downServices.service(i).error_text();

            if (info->error->exit != 0)
                down_services.emplace_back(info);
        }
    }

    void Manager::publishMessage(const PodDescriptor &info) const {

        if (publisher_ == nullptr){
            std::cerr << "No Publisher" << std::endl;
            return ;
        }

        std::string message = info.toGossipMessage();
        auto res = publisher_->publishMessage(usr_info + " " + message);

        if(!res)
            std::cerr << "Publish Failed" << std::endl;
    }

    WorkerDescriptor *Manager::connectToWorker(WorkerDescriptor *wd) {
        wd->session = cli.createSessionOnNewChannel(IPAddress(wd->ip.c_str(), AF_INET, wd->port));
        wd->alive = wd->session->connected();

        return wd;
    }

    bool Manager::snapshot() {

        // 对snapshot的状态进行复制
        auto workers_ = workers;
        auto services_ = pods;
        int cur_line_ = cur_line;

        snapshot_thread = std::thread([workers_, services_, cur_line_]() {

            std::ofstream os("./snapshot.backup", std::ios::out);
            if (!os.is_open())
                return;

            os << cur_line_ << std::endl;

            for (auto &wd: workers_) {
                os << wd->ip << " " << wd->port << " " << wd->alive << " " << wd->session->lastRpcTime()
                   << std::endl;
            }
            os << "WorkerDescriptorEND" << std::endl;

            for (auto &sd: services_) {
                os << sd.second.toSnapshotMessage() << std::endl;
            }
            os << "ServiceDescriptorEND" << std::endl;

            std::filesystem::rename("./snapshot.backup", "./snapshot");
        });

        return true;
    }

    void Manager::snapshotToServiceInfo(const string &snapshot_line, PodDescriptor &info) {
        std::stringstream ss(snapshot_line);
        ss >> info.service_ >> info.alias_;
        std::string type;
        ss >> type;
        info.type_ = (type == "host" ? host : docker);
        ss >> info.port_;
        ss >> info.alive_;

        std::string ip;
        int port;
        ss >> ip >> port;
        info.wd_ = findWorkerDescriptor(ip, port);

        std::string params;

        ss >> params;

        std::stringstream sss(params.substr(1, params.size() - 2));

        while (std::getline(sss, params, ',')) {
            auto param = new std::string(params);
            info.exe_params_.emplace_back(const_cast<char *>(param->c_str()));//这里会出问题
        }

        ss >> params;

        std::stringstream ssss(params.substr(1, params.size() - 2));
        while (std::getline(ssss, params, ',')) {
            auto param = new std::string(params);
            info.docker_params_.emplace_back(const_cast<char *>(param->c_str()));
        }

        ss >> info.restart_;
    }

    void Manager::parseLogLine(const string &log_line) {
        std::stringstream ss(log_line);
        std::string word;
        ss >> word;

        std::vector<std::string> vec = {"CSS", "DSS"};
        if (std::find(vec.begin(), vec.end(), word) != vec.end())
            return;

        if (word == "DSE") {
            std::string alias, result;
            ss >> alias >> result;
            if (result == "Success")
                pods.erase(alias);
        } else if (word == "CSE") {
            // CSE hello my_hello host 1 127.0.0.1:8989 8989 {} {} 0
            PodDescriptor info;
            std::getline(ss, word, '\n');

            std::stringstream stream(word);
            std::string alias, result;
            stream >> alias >> result;
            if (result == "Fail")
                return;

            snapshotToServiceInfo(word, info);
            pods.emplace(info.alias_, info);
        }

    }

    void Manager::recover() {

        // 观察有没有 snapshot.backup.
        if (std::filesystem::exists("./snapshot.backup"))
            std::filesystem::rename("./snapshot.backup", "./snapshot");
        // 观察有没有 snapshot
        if (std::filesystem::exists("./snapshot")) {

            std::ifstream is("./snapshot");
            std::string line;

            std::getline(is, line);
            cur_line = std::stoi(line);     // cur_line

            while (std::getline(is, line) && line != "WorkerDescriptorEND") {
                std::stringstream ss(line);
                auto *wd = new WorkerDescriptor;
                ss >> wd->ip >> wd->port >> wd->alive;
                workers.emplace_back(wd);
            }

            while (std::getline(is, line) && line != "ServiceDescriptorEND") {
                std::stringstream ss(line);
                PodDescriptor info;
                snapshotToServiceInfo(line, info);
                pods.emplace(info.alias_, std::move(info));
            }
        }

        if (std::filesystem::exists("./manager.log")) {
            std::ifstream is("./manager.log");
            std::string line;
            int l = 0;

            while (std::getline(is, line)) {
                if (++l <= cur_line)
                    continue;

                parseLogLine(line);
            }
        }


        for (auto &worker: workers) {
            connectToWorker(worker);
        }

        for (auto &service: pods) {
            auto &info = service.second;
            auto res = createService(info);    // 无论是否还存活，都发送出fork请求，具体怎么做由worker决定（决策由最底层决定）
            if (res.isFail()) {
                info.alive_ = false;
                down_services.emplace_back(&info);
            }
        }

        if (snapshot()) {
            if (snapshot_thread.joinable())
                snapshot_thread.join();

            std::filesystem::remove("./manager.log");
            cur_line = 0;
        }
    }

    std::pair<bool, std::string>
    Manager::shutdownPod(WorkerDescriptor *wd, std::string service, std::string alias, ServiceType type) {

        ShutdownInput input;
        auto sd = input.sd().New();
        sd->set_service(service);
        sd->set_alias(alias);
        sd->set_type(type);
        input.set_user_info(usr_info);
        input.set_allocated_sd(sd);


        cur_line = logger->append("DSS " + alias);
        auto rpc_res = wd->session->run(SHUTDOWN, &input);

        if (rpc_res.isFailed()) {
            cur_line = logger->append("DSE " + alias + " Fail");
            return {false, rpc_res.ErrorText()};
        }

        Echo echo;
        rpc_res.castMessageTo(&echo);

        if (echo.fail()) {
            if (echo.error_text() == "Manager Down") {
                // 这里应该没有问题，外层这个 service 已经被删除掉了，其他的服务转为下线状态

                for (auto &serv: wd->pods) {
                    serv->alive_ = false;
                    down_services.emplace_back(serv);
                }
                wd->pods = {};  // 将当前 worker 的服务设置为下线

            } else {
                cur_line = logger->append("DSE " + alias + " Fail");
                return {false, echo.error_text()};
            }
        }
        cur_line = logger->append("DSE " + alias + " Success");


        return {true, echo.error_text()};
    }

    std::vector<const PodDescriptor *> Manager::showServices(const string &service) {

        std::vector<const PodDescriptor *> servs;

        if (service == "*") {
            for (auto &serv: pods)
                servs.emplace_back(&serv.second);
        } else {
            auto it = pods.find(service);
            if (it != pods.end())
                servs.emplace_back(&it->second);
        }

        return servs;
    }
}