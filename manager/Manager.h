//
// Created by 唐仁初 on 2022/7/28.
//

#ifndef TINYSWARM_MANAGER_H
#define TINYSWARM_MANAGER_H

#include "WorkerDescriptor.h"
#include "WorkerClient.h"
#include "ServiceImplInfo.h"
#include "LogQueue.h"
#include "RPCServer.h"

#include <sw/redis++/redis++.h>
#include <list>
#include <sstream>

class Manager {

public:

    // 错误处理： 如果是任务出现异常，那么使用check rpc来收集错误信息，并且尝试重启
    //           如果是worker节点出现异常，那么将worker节点上的全部任务调度到其他地方

    WorkerDescriptor *selectWorker() {
        // 从 redis 里面得到一个合适的 worker 结点

        std::string str; // get from redis

        for (auto &worker: workers) {
            if (worker->ip == str) {
                return worker;
            }
        }

        return {};
    }


    //FIXME : 由于重试操作，因此所有的rpc 必须在调用rpc 前就将 service 在内存中完成修改



    /// RPC, Create a service on a worker node with WorkDescriptor. If succeed, current node
    /// will publish message to other manager nodes.
    /// \param wd WorkerDescriptor of worker node to create service.
    /// \param service The service to create.
    /// \param alias The alias of service.
    /// \param type The type of service.
    /// \param exe_params The params of service.
    /// \param docker_params The docker start params, if service run in docker, th
    /// \param restart The time service will restart, -1 means whenever.
    /// \return [ if created, error info ]
    std::pair<bool, std::string>
    createService(WorkerDescriptor *wd, const std::string &service, const std::string &alias,
                  ServiceType type, const std::vector<char *> &exe_params,
                  const std::vector<char *> &docker_params, int restart = 0) {

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
        ServiceImplInfo info(service, alias, type, 1, wd, exe_params, docker_params, restart);

        auto res = services.emplace(alias, info);    // 这里优先操作内存，因为容易恢复

        if (!res.second) {
            return {false, "services emplace error"};
        }

        // log << create service ...
        cur_line = logger->append("CSS " + alias);

        retry :
        auto rpc_res = wd->session->run(FORK, &input);
        // log << create success

        if (rpc_res.isFailed()) {
            cur_line = logger->append("CSE " + alias + " Fail");
            services.erase(res.first);  // 如果失败则进行回退
            return {false, rpc_res.ErrorText()};
        }

        ForkEcho echo;
        rpc_res.castMessageTo(&echo);

        if (echo.fail()) {
            if (echo.error_text() == "Manager Down") {

                std::cout << "receive manager down" << std::endl;

                for (auto &serv: wd->services) {
                    serv->alive_ = false;
                    down_services.emplace_back(serv);
                }

                wd->services = {};  // 将当前 worker 的服务设置为下线

                usleep(100);
                goto retry;     //FIXME: 这里能够正常使用吗？

            } else {
                cur_line = logger->append("CSE " + alias + " Fail");
                services.erase(res.first);
                return {false, echo.error_text()};
            }
        }

        // FIXME : 这里需要持久化，防止崩溃。
        cur_line = logger->append("CSE " + info.toSnapshotMessage());

        wd->services.emplace_back(&info);

        publishMessage(res.first->second);

        return {true, echo.sd().alias()};
    }

    /// RPC, Create a service on a worker node with ServiceInfo. If succeed, current node
    /// will publish message to other manager nodes. The ServiceInfo will be set alive.
    /// \param info ServiceInfo of service to create. Warning This ServiceInfo is in services map.
    /// \return [if created, error info]
    std::pair<bool, std::string> createService(ServiceImplInfo &info) {

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
            return {false, rpc_res.ErrorText()};
        }

        ForkEcho echo;
        rpc_res.castMessageTo(&echo);

        if (echo.fail()) {

            if (echo.error_text() == "Manager Down") {

                for (auto &service: info.wd_->services) {
                    service->alive_ = false;
                    down_services.emplace_back(service);
                }
                info.wd_->services = {};  // 将当前 worker 的服务设置为下线

                goto retry;     //FIXME: 这里能够正常使用吗？
            } else {
                cur_line = logger->append("CSE " + info.alias_ + " Fail");
                return {false, echo.error_text()};
            }
        }
        // log << create success
        cur_line = logger->append("CSE " + info.toSnapshotMessage());

        publishMessage(info);

        info.alive_ = true;
        return {true, echo.sd().alias()};
    }

    std::pair<bool, std::string> createHostService(WorkerDescriptor *wd,
                                                   const std::string &service,
                                                   const std::string &alias,
                                                   const std::vector<char *> &exe_params,
                                                   int restart = 0) {
        return createService(wd, service, alias, host, exe_params, {}, restart);
    }

    std::pair<bool, std::string>
    createDockerService(WorkerDescriptor *wd, const std::string &service, const std::string &alias,
                        const std::vector<char *> &exe_params, const std::vector<char *> &docker_params,
                        int restart = 0) {
        return createService(wd, service, alias, docker, exe_params, docker_params, restart);
    }

    /// RPC, Force shutdown a service in worker node.No any other actions.Used by stopService and transferService.
    /// \param wd WorkerDescriptor of worker node to shutdown service.
    /// \param service The name of service.
    /// \param alias The alias of service.
    /// \param type The type of service.
    /// \return [if created, error info]
    std::pair<bool, std::string>
    shutdownService(WorkerDescriptor *wd, std::string service, std::string alias, ServiceType type) {

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

                for (auto &serv: wd->services) {
                    serv->alive_ = false;
                    down_services.emplace_back(serv);
                }
                wd->services = {};  // 将当前 worker 的服务设置为下线

            } else {
                cur_line = logger->append("DSE " + alias + " Fail");
                return {false, echo.error_text()};
            }
        }
        cur_line = logger->append("DSE " + alias + " Success");


        return {true, echo.error_text()};
    }


    /// Median Interface, force shutdown a service using ServiceInfo.Atomic Function.
    /// \param info
    /// \return
    std::pair<bool, std::string> shutdownService(ServiceImplInfo &info) {
        if (!info.alive_) {
            return {false, "To shutdown a dead service"};
        }
        auto res = shutdownService(info.wd_, info.service_, info.alias_, info.type_);
        info.alive_ = res.first;
        // info.wd_->removeService(&info);  在最外层调用
        return res;
    }

    /// User Interface, force shutdown a service using ServiceInfo.Atomic Function.
    /// \param info ServiceInfo of service to stop.
    /// \return If Succeed.
    bool stopService(ServiceImplInfo &info) {

        auto it = services.find(info.alias_);
        if (it == services.end())
            return false;

        services.erase(it); // 删除 service 本身信息
        info.wd_->removeService(&info); // 删除 worker 上的指针

        auto[rpc_res, error] = shutdownService(info);

        if (rpc_res) {

            if (!info.alive_) {   // 如果在用户命令前该服务已经停止了，那么需要将其从失活链表中取出
                removeDownService(&info);
            }

            return true;
        }

        services.emplace(info.alias_, info);    // 失败则进行恢复
        info.wd_->services.emplace_back(&info);
        std::cerr << error << std::endl;
        return false;
    }

    ///FIXME: 这个函数不能保证操作的原子性，因为两个都是 RPC，考虑用 2PC？
    bool transferService(ServiceImplInfo &info, WorkerDescriptor *new_wd) {

        auto old_info = info;
        info.wd_ = new_wd;

        auto[res, error] = createService(info);

        if (!res) {
            std::cerr << error << std::endl;
            info = old_info;
            return false;
        }

        if (old_info.alive_ == 1) {
            // 如果还存活的话，需要先关闭。
            old_info.wd_->removeService(&info);

            auto[res1, error1] = shutdownService(old_info);

            if (!res1) {
                std::cerr << error << std::endl;    // 这里失败的话，直接将service从manager 去除，一段时间后worker也会去除
            }
        }

        //FIXME: 这里如果失败会导致直接崩溃
        info.wd_->services.emplace_back(&info);

        return true;
    }

    /// Collect Error Information If Service Is Down.
    /// \param wd WorkerDescriptor of worker node.
    /// \param line The last lines of output of down service.
    void checkServices(WorkerDescriptor *wd, int line) {
        if (!wd->alive)
            return;

        CheckInput input;
        input.set_line(line);
        input.set_user_info(usr_info);
        DownServices downServices;
        std::string error_text;

        auto rpc_res = wd->session->run(CHECK,&input);

        if (rpc_res.isFailed()) {
            wd->alive = false;  // 如果 worker 没有存活，将所有的服务转为下线
            for (auto &service: wd->services) {
                service->alive_ = false;
                down_services.emplace_back(service);
            }
            return;
        }

        if (downServices.service(0).alias() == "*") {
            for (auto &service: wd->services) {
                service->alive_ = false;
                service->error = new ServiceInfoError();
                service->error->exit = downServices.service(0).exit_num();
                service->error->out_file = downServices.service(0).out_file();
                service->error->reason = downServices.service(0).error_text();
            }
            return;
        }

        for (int i = 0; i < downServices.service_size(); i++) {
            auto alias = downServices.service(i).alias();
            auto info = getServiceInfoIterByAlias(alias);
            if (info == nullptr) {
                std::cerr << "No info named alias: " << alias << std::endl;
                continue;
            }
            info->alive_ = false;
            info->error = new ServiceInfoError();
            info->error->exit = downServices.service(i).exit_num();
            info->error->out_file = downServices.service(i).out_file();
            info->error->reason = downServices.service(i).error_text();

            if (info->error->exit != 0)
                down_services.emplace_back(info);
        }
    }

    void checkServicesOnAllWorker(int line) {
        for (auto worker: workers) {
            if (worker->alive)
                checkServices(worker, line);
            else
                worker->alive = worker->session->reconnect();
        }
    }

    void publishMessage(const ServiceImplInfo &info) const {
        std::string message = info.toGossipMessage();
        return publishMessage(message);
    }

    // 要求最后收到的 message 格式 "manager x.x.x.x port service alias"
    void publishMessage(const std::string &message) const try {

        if (!redis)
            return;

        redis->publish("manager", usr_info + " " + message);

    } catch (std::exception &e) {
        std::cerr << "redis not connected." << std::endl;
    }

    WorkerDescriptor *connectToWorker(IPAddress ipAddress) {
        auto session = cli.createSessionOnNewChannel(ipAddress);

        auto wd = new WorkerDescriptor;
        wd->ip = ipAddress.getAddressWithoutPort();
        wd->port = std::stoi(ipAddress.getPort());
        wd->session = session;
        wd->alive = true;

        workers.emplace_back(wd);
        return wd;
    }

    WorkerDescriptor *connectToWorker(WorkerDescriptor *wd) {
        wd->session = cli.createSessionOnNewChannel(IPAddress(wd->ip.c_str(), AF_INET, wd->port));
        wd->alive = wd->session->alive();

        return wd;
    }

    /// Try to Do failed operators.
    void tryDoFailedFunctors() {
        std::list<std::function<bool()>> functors;
        {
            std::lock_guard<std::mutex> lg(functor_mtx);
            std::swap(functors, failed_functors);
        }

        for (auto functor = functors.begin(); functor != functors.end();) {
            if ((*functor)()) {
                functor = functors.erase(functor);
            } else {
                functor++;
            }
        }

        if (!functors.empty()) {
            std::lock_guard<std::mutex> lg(functor_mtx);
            failed_functors.insert(failed_functors.end(), functors.begin(), functors.end());
        }

    }


    // 由于是一个单线程的事务，所以开始snapshot的时候，其他事务要么失败，要么成功，无中间态
    bool snapshot() {

        // 对snapshot的状态进行复制
        auto workers_ = workers;
        auto services_ = services;
        int cur_line_ = cur_line;

        snapshot_thread = std::thread([workers_, services_, cur_line_]() {

            std::ofstream os("./snapshot.backup", std::ios::out);
            if (!os.is_open())
                return;

            os << cur_line_ << std::endl;

            for (auto &wd: workers_) {
                os << wd->ip << " " << wd->port << " " << wd->alive << " " << wd->session->lastRpcTime() << std::endl;
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

    /// Find WorkerDescriptor using ip and port.
    /// \param ip ip of WorkerDescriptor.
    /// \param port port of WorkerDescriptor.
    /// \return ptr of WorkerDescriptor or nullptr if not found.
    WorkerDescriptor *findWorkerDescriptor(const std::string &ip, int port) {
        for (auto &wd: workers) {
            if (wd->ip == ip && wd->port == port)
                return wd;
        }
        return nullptr;
    }

    /// Find ServiceInfo using alias.
    /// \param alias alias of service
    /// \return ptr of ServiceInfo or nullptr if not found.
    ServiceImplInfo *getServiceInfoIterByAlias(const std::string &alias) {
        auto it = services.find(alias);
        return it == services.end() ? nullptr : &it->second;
    }


    void snapshotToServiceInfo(const std::string &snapshot_line, ServiceImplInfo &info) {
        std::stringstream ss(snapshot_line);
        ss >> info.service_ >> info.alias_;
        std::string type;
        ss >> type;
        info.type_ = (type == "host" ? host : docker);
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

    void parseLogLine(const std::string &log_line) {
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
                services.erase(alias);
        } else if (word == "CSE") {
            // CSE hello my_hello host 1 127.0.0.1:8989 8989 {} {} 0
            ServiceImplInfo info;
            std::getline(ss, word, '\n');

            std::stringstream stream(word);
            std::string alias, result;
            stream >> alias >> result;
            if (result == "Fail")
                return;

            snapshotToServiceInfo(word, info);
            services.emplace(info.alias_, info);
        }

    }

    // 这里无论断连的时间是多少，只要重新将服务放回到原来位置上就可以，具体怎么做由worker来决定
    void recover() {

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
                ServiceImplInfo info;
                snapshotToServiceInfo(line, info);
                services.emplace(info.alias_, std::move(info));
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

        for (auto &service: services) {
            auto &info = service.second;
            auto[res, error] = createService(info);    // 无论是否还存活，都发送出fork请求，具体怎么做由worker决定（决策由最底层决定）
            if (!res) {
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

    ~Manager() {
        if (snapshot_thread.joinable())
            snapshot_thread.join();
    }

    explicit Manager(const std::string &usr) : usr_info(usr), cur_line(0) {

        logger = std::make_unique<LogQueue>("./manager.log");
        logger->start();

        // 从snapshot 和 log 中恢复之前的信息
        recover();





        // 创建一个redis实例以及redis的客户端
        redis = std::make_unique<sw::redis::Redis>("tcp://127.0.0.1:6379");
//        // 向注册中心得到当前存活的节点
//
//
//
//
//        IPAddress ipAddress(AF_INET, 8989);
//        std::vector<std::function<void()>> functors = {
//                [this] {
//                    auto now = time_t(nullptr);
//                    if (now > check_time) {
//                        checkServicesOnAllWorker(10);
//                        check_time = now + 5;
//                    }
//                }
//        };
//        server = std::make_unique<RPCServer>(ipAddress, functors, 1, -1);
    }


    /// Delete DownService when it is up again or stopped by user.
    /// \param service Ptr of DownService.
    void removeDownService(ServiceImplInfo *service) {
        for (auto it = down_services.begin(); it != down_services.end();) {
            if (*it == service) {
                down_services.erase(it);
                return;
            } else {
                it++;
            }
        }
    }


    void transferDownService() {

        for (auto service: down_services) {

            // 如果没有存活，尝试转移到另外一个地方
            if (!service->wd_->alive) {

                auto wd = selectWorker();
                auto res = transferService(*service, wd);  // 转移 service 到另外一个 worker 上
                if (res) {
                    removeDownService(service);
                }

            } else {

                auto[res, error] = createService(*service);    // 当前节点如果还存活，则直接对服务进行重启
                if (res)
                    removeDownService(service);
            }
        }


    }


    auto getServices() {
        return &services;
    }


    void run() const {
        server->startService();
    }

    std::vector<const ServiceImplInfo *> showServices(const std::string &service) {

        std::vector<const ServiceImplInfo *> servs;

        if (service == "*") {
            for (auto &serv: services)
                servs.emplace_back(&serv.second);
        } else {
            auto it = services.find(service);
            if (it != services.end())
                servs.emplace_back(&it->second);
        }

        return servs;
    }


public:
    // gossip_node :
    // redis prototype :
    std::unique_ptr<sw::redis::Redis> redis;

    // log utils
    std::thread snapshot_thread;
    std::unique_ptr<LogQueue> logger;
    int cur_line;   //FIXME:这里行数没有自加的功能

    // worker nodes
    std::list<WorkerDescriptor *> workers;  // 链表比较短，并且用的机会少，所以存活和非存活可以放在一起

    WorkerClient cli;


    /// 所有的服务信息存储在services表中，其余的只是提供指针
    /// 增加操作: createService,删除操作: stopService,转移操作: transferService
    using Alias = std::string;
    std::map<Alias, ServiceImplInfo> services;  // 用于 Service 的查找(暴露给用户方便)。
    std::list<ServiceImplInfo *> down_services; // 用于 Manager 内部的重连操作，用户可以定义是否允许迁移等。

    std::mutex functor_mtx;
    std::list<std::function<bool()>> failed_functors;    // 失败的

    std::string usr_info;

    std::unique_ptr<RPCServer> server;

    time_t check_time;
};


#endif //TINYSWARM_MANAGER_H
