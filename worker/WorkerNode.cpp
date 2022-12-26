//
// Created by 唐仁初 on 2022/7/27.
//

#include "WorkerNode.h"
#include "probe/LiveProbe.h"


namespace tiny_swarm::worker {

    WorkerNode::WorkerNode(const std::string &work_dir) : work_dir_(work_dir), probe_(new probe::TCPProbe) {

        if (!std::filesystem::exists(work_dir)) {
            std::cerr << "Work Directory Not Exits." << std::endl;
            exit(1);
        }

        if (work_dir.back() == '/') {
            status = new WorkerStatus(work_dir, work_dir + "WorkerInformation");
            recorder = std::make_unique<recorder::PodStartRecorder>(work_dir + "ServiceRecord", rpc_service);
        } else {
            status = new WorkerStatus(work_dir, work_dir + "/WorkerInformation");
            recorder = std::make_unique<recorder::PodStartRecorder>(work_dir + "/ServiceRecord", rpc_service);
        }


        // 这里要进行docker的注册，防止后面pull失败
        signal(SIGCHLD, childExitHandler);


        // 设置周期性的任务
        std::vector<std::function<void()>> functors = {
                [this] { checkIfPodsRunning(); },    // 检查进程是否异常退出
                [this] { updateStatus(); },     // 更新 worker 节点信息
                [this] { removeManagerNotUsed(10); },    // 定期移除过期 manager
                [this] { checkPodsUsingProbe(); }
        };


        rpc_service = new RPCInterface(this);
        rpc_server = new RPCServer(IPAddress(AF_INET, 8989), functors);
        rpc_server->addService(rpc_service);
    }

    WorkerNode::~WorkerNode() {

        delete status;
        delete rpc_service;
        delete rpc_server;
        delete probe_;

        // 析构函数是被视为正常退出的
        // 退出时关闭所有子进程以及输出文件
        for (auto &pod: pods) {
            kill(pod.pid, SIGKILL);
            std::filesystem::remove(pod.out_file);
        }
    }

    pid_t WorkerNode::createProcess(const std::string &service, const std::string &alias, const std::string &manager,
                                    int type, int port, std::vector<char *> exe_params,
                                    std::vector<char *> docker_params, std::string &error_text, int restart) {

        // FIXME: 判断端口号是否能够使用


        // 打开一个 docker 类型的服务
        if (type == 1) {

            auto out_file = work_dir_;
            out_file.append(service).append(
                    manager + "_" + std::to_string(time(nullptr)) + std::to_string(pods.size()));

            // 如果没有指定容器名称，那么本地需要指定一个容器名用于管理
            std::string container_name;

            for (auto it = docker_params.begin(); it != docker_params.end(); it++) {
                if (strcmp(*it, "--name") == 0) {
                    container_name = *(++it);
                    break;
                }
            }

            std::vector<char *> docker_command = {const_cast<char *>("/usr/local/bin/docker"),
                                                  const_cast<char *>("run")};
            if (container_name.empty()) {
                container_name = alias;
                docker_command.emplace_back(const_cast<char *>("--name"));
                docker_command.emplace_back(const_cast<char *>(container_name.c_str()));
            }
            docker_command.insert(docker_command.end(), docker_params.begin(), docker_params.end());
            docker_command.emplace_back(const_cast<char *>(service.c_str()));
            docker_command.insert(docker_command.end(), exe_params.begin(), exe_params.end());
            docker_command.emplace_back(nullptr);


            pid_t pid = fork();

            if (pid == 0) {

                int fd = open(out_file.c_str(), O_CREAT | O_WRONLY, S_IRWXU); // 这里要增加权限，否则fork出的子进程无法拥有文件所有权
                int res1 = dup2(fd, 1);  // 重定向输出
                int res2 = dup2(fd, 2);

                if (res1 == false || res2 == false) {
                    error_text = DUP_ERROR;
                    exit(-1);
                }


                close(fd);

                std::cout << service << " " << alias << " " << manager << " " << type << std::endl << std::endl
                          << std::endl;

                auto res = execv("/usr/local/bin/docker", &docker_command[0]);

                if (res == -1) {

                    error_text = "Error to Open Service: " + service +
                                 "\nerrno " + std::to_string(errno) +
                                 "\nOutput File: \n";

                    std::ifstream is(out_file);
                    error_text.append((std::istreambuf_iterator<char>(is)),
                                      std::istreambuf_iterator<char>());
                    exit(-1);
                }

            } else {

                std::lock_guard<std::mutex> lg(mtx1);
                pods.emplace_back(
                        PodDescriptor{pid, type, service, alias, container_name, manager, out_file, exe_params,
                                      docker_params, port, restart});

                std::cout << "catch new task\n";

                auto md = getOrAddManager(manager);
                md->pods.emplace_back(&pods.back());


            }

            return pid;
        }


        // 打开一个 host 类型的服务
        if (type == 0) {
            // 如果这里的exec 不存在，直接返回一个异常
            std::filesystem::path path(work_dir_);
            path.append(service).append(service);
            if (!std::filesystem::exists(path)) {
                error_text = SERV_NO_EXE;
                return -1;
            }


            // 这里要完成的工作： 分配一个输出的文件，判断运行方式，

            auto exec = work_dir_;
            exec.append(service).append(service);

            auto out_file = work_dir_;
            out_file.append(service).append(
                    manager + "_" + std::to_string(time(nullptr)) + std::to_string(pods.size()));


            pid_t pid = fork();

            if (pid == 0) {

                //exe_params.emplace(exe_params.begin(),const_cast<char*>(out_file.c_str()));
                exe_params.emplace(exe_params.begin(), const_cast<char *>(exec.c_str()));
                //exe_params.emplace(exe_params.begin(),const_cast<char*>("sh"));
                exe_params.emplace_back(nullptr);

                int fd = open(out_file.c_str(), O_CREAT | O_WRONLY, S_IRWXU); // 这里要增加权限，否则fork出的子进程无法拥有文件所有权
                int res1 = dup2(fd, 1);  // 重定向输出
                int res2 = dup2(fd, 2);

                if (res1 == false || res2 == false) {
                    error_text = DUP_ERROR;
                    exit(-1);
                }

                close(fd);

                std::cout << service << " " << alias << " " << manager << " " << type << std::endl << std::endl
                          << std::endl;

                auto res = execv(exec.c_str(), &exe_params[0]);

                if (res == -1) {
                    error_text = "Error to Open Service: " + service +
                                 "\nerrno " + std::to_string(errno) +
                                 "\nOutput File: \n";

                    std::ifstream is(out_file);
                    error_text.append((std::istreambuf_iterator<char>(is)),
                                      std::istreambuf_iterator<char>());
                    exit(-1);
                }

            } else {
                std::lock_guard<std::mutex> lg(mtx1);
                auto res = pods.emplace_back(
                        PodDescriptor{pid, type, service, alias, exec, manager, out_file, exe_params, docker_params,
                                      port, restart});

                std::cout << "catch new task\n";

                auto md = getOrAddManager(manager);
                md->pods.emplace_back(&pods.back());

            }
            return pid;
        }

        return -1;
    }

    pid_t WorkerNode::createProcess(const PodDescriptor &pd, std::string &error_text) {
        return createProcess(pd.service, pd.alias, pd.manager, pd.type, pd.port_, pd.exe_params, pd.docker_param,
                             error_text,
                             pd.restart);
    }

    bool WorkerNode::stopPod(PodDescriptor *pd) {
        return pd->type == 0 ? stopHostPod(pd->pid) : stopDockerPod(pd->pid);
    }

    bool WorkerNode::stopHostPod(pid_t pid) {

        std::cout << "ready to shut down service\n";
        std::lock_guard<std::mutex> lg(mtx1);

        for (auto pod = pods.begin(); pod != pods.end(); pod++) {
            if (pod->pid == pid) {
                kill(pid, SIGKILL);

                auto res = std::filesystem::remove(pod->out_file);
                if (!res)
                    unused_files.emplace_back(pod->out_file);

                // 这里是内部接口，不需要删除 manager 信息
//                auto md = &managers.find(service->manager)->second;
//                deleteServiceInManagerDescriptor(md, service->pid);

                recorder->removePodRecord(pod->alias);    // 删除启动文件
                pods.erase(pod);
                return true;
            }
        }
        return false;
    }

    int WorkerNode::stopPod(const std::string &alias, const std::string &manager) {
        std::lock_guard<std::mutex> lg(mtx1);

        for (auto pod = pods.begin(); pod != pods.end(); pod++) {
            if (pod->alias == alias) {

                if (pod->manager != manager)
                    return 1;

                if (-1 == kill(pod->pid, SIGKILL))
                    return -1;


                auto md = getOrAddManager(pod->manager);    // 清除 ManagerDescriptor
                deletePodInManagerDescriptor(md, pod->pid);

                recorder->removePodRecord(alias); // 清除启动信息
                auto res = std::filesystem::remove(pod->out_file);  // 清除输出文件
                if (!res)
                    unused_files.emplace_back(pod->out_file);

                pods.erase(pod);
                return 0;
            }
        }
        return -2;
    }

    bool WorkerNode::stopDockerPod(pid_t pid) {

        std::string command;
        std::lock_guard<std::mutex> lg(mtx1);

        for (auto pod = pods.begin(); pod != pods.end(); pod++) {
            if (pod->pid == pid) {
                command = "docker stop " + pod->exec;
                auto res = std::filesystem::remove(pod->out_file);
                if (!res)
                    unused_files.emplace_back(pod->out_file);

//                auto md = &managers.find(service->manager)->second;
//                deleteServiceInManagerDescriptor(md, service->pid);

                recorder->removePodRecord(pod->alias);    // 删除启动文件
                pods.erase(pod);
                break;
            }
        }
        system(command.c_str());

        return kill(pid, SIGKILL) == 0;
    }

    int WorkerNode::stopDockerPod(const std::string &alias, const std::string &manager) {

        std::string command;
        std::lock_guard<std::mutex> lg(mtx1);

        for (auto pod = pods.begin(); pod != pods.end(); pod++) {
            if (pod->alias == alias) {
                if (pod->manager != manager)
                    return 1;

                command = "docker stop " + pod->exec;

                recorder->removePodRecord(alias); // 清除启动信息
                auto res = std::filesystem::remove(pod->out_file);  // 清除输出文件

                if (!res)
                    unused_files.emplace_back(pod->out_file);
                pods.erase(pod);
                system(command.c_str());    //FIXME: 这里不能确保docker被关闭掉的

                int kill_res = kill(pod->pid, SIGKILL);

                if (0 == kill_res) {
                    auto md = getOrAddManager(pod->manager);    // 清除 ManagerDescriptor
                    deletePodInManagerDescriptor(md, pod->pid);
                }

                return kill_res;

            }
        }
        return -2;
    }

    bool WorkerNode::checkIfAliasOK(const std::string &manager, const std::string &alias, std::string &reason) {
        for (auto &pod: pods) {
            if (pod.alias == alias) {
                reason = (pod.manager == manager) ? SAME_SERV : SAME_ALIAS;
                return false;
            }
        }
        return true;
    }

    void WorkerNode::checkIfPodsRunning() {
        std::list<std::shared_ptr<ExitInformation>> error_infos;

        {
            std::shared_ptr<ExitInformation> info = nullptr;

            do{
                info = infos.pop();
                error_infos.emplace_back(info);
            }
            while(info != nullptr);
        }

        for (auto &info: error_infos) {
            std::lock_guard<std::mutex> lg(mtx1);

            for (auto pod = pods.begin(); pod != pods.end();) {

                if (pod->pid == info->pid) {

                    // 删除掉ManagerDescriptor中的记录
                    auto md = getOrAddManager(pod->manager);
                    deletePodInManagerDescriptor(md, info->pid);

                    // 删除对应的 ServiceRecord
                    recorder->removePodRecord(pod->alias);

                    // 重启进程
                    if (pod->restart-- > 0) {
                        std::cout << "restart" << std::endl;
                        std::string error;
                        PodDescriptor sd = *pod;
                        pods.erase(pod);
                        createProcess(sd, error);
                        break;
                    }

                    // 不需要重启进程
                    if (pod->type == 0) {
                        // 对于 host 进程，直接通过进程代码来判断

                        std::lock_guard<std::mutex> lg1(mtx2);
                        down_pods.emplace_back(
                                PodExitInformation{pod->alias, pod->manager, pod->out_file,
                                                   info->exit_code, *pod});

                    } else {
                        // 对于 docker 进程
                        //FIXME : 这里用一个 shell 脚本来解决


                    }

                    std::cout << "erased " << info->pid << " status " << info->exit_code << std::endl;
                    pods.erase(pod);
                    break;

                } else {
                    pod++;
                }
            }
        }
    }

    void WorkerNode::checkPodsUsingProbe() {
        auto now = time(nullptr);

        for (auto pod = pods.begin(); pod != pods.end();) {

            auto probe = pod->probe_;

            if (now - probe->lastProbeTime() < 5) {
                continue;
            }

            auto live = probe->isPeerAlive(IPAddress("127.0.0.1", AF_INET, pod->port_));

            if (live) {
                pod++;

            } else {
                down_pods.emplace_back(
                        PodExitInformation(pod->alias, pod->manager, pod->out_file, -1, *pod, "BLOCKED"));
                auto pid = pod->pid;
                pods.erase(pod);
                kill(pid, SIGINT);
            }
        }
    }

    void WorkerNode::collectExitInformation(const std::string &manager, std::list<PodExitInformation> &exits) {

        updateManagerTimestamp(manager); // 更新timestamp

        // 如果worker在两次check中崩溃且重启了，那么要检查
        recorder->checkPodRecord(manager, exits);

        std::lock_guard<std::mutex> lg(mtx2);

        for (auto pod = down_pods.begin(); pod != down_pods.end();) {
            if (pod->manager == manager) {
                exits.emplace_back(*pod);
                pod = down_pods.erase(pod);
            } else {
                pod++;
            }
        }
    }

    void WorkerNode::cleanDumpFiles(const std::list<PodExitInformation> &exits) {
        for (const auto &e: exits) {
            std::filesystem::remove(e.out_file);    // 删除进程的输出文件
            recorder->removePodRecord(e.alias);    // 删除进程的启动信息
        }
    }

    bool WorkerNode::isManagerDown(const std::string &manager) {
        auto it = managers.find(manager);

        if (it == managers.end())
            return false;

        auto res = it->second.status != 0;
        it->second.status = 0;

        return res;
    }

    void WorkerNode::updateManagerTimestamp(const std::string &manager) {
        auto it = managers.find(manager);
        if (it == managers.end())
            return;

        it->second.timestamp = time(nullptr);
        it->second.status = 0;  // 这代表manager重新被激活了
    }

    ManagerDescriptor *WorkerNode::getOrAddManager(const std::string &manager) {
        auto it = managers.find(manager);
        if (it == managers.end()) {
            auto res = managers.emplace(manager, ManagerDescriptor(manager));
            return &res.first->second;
        }

        it->second.timestamp = time(nullptr);
        it->second.status = 0;  // 这代表manager重新被激活了

        return &it->second;
    }

    bool WorkerNode::deletePodInManagerDescriptor(ManagerDescriptor *md, pid_t pid) {
        for (auto pod = md->pods.begin(); pod != md->pods.end(); pod++) {
            if ((*pod)->pid == pid) {
                md->pods.erase(pod);
                return true;
            }
        }
        return false;
    }

    void WorkerNode::removeManagerNotUsed(int time_s) {

        if (time_s < 0)
            return;

        auto now = time(nullptr);
        for (auto &pair: managers) {
            auto &md = pair.second;

//            for (auto service: md.services) {
//                std::cout << service->pid << std::endl;
//            }

            if (md.status == 0 && now - md.timestamp > time_s) {

                for (auto pod: md.pods) {
                    // 停止所有的服务
                    std::cout << "calling force stop " << pod->pid << std::endl;
                    stopPod(pod);
                }
                md.pods.clear();

                md.status = -1; // 将 manager 的状态设置为掉线
                return;
            }
        }
    }

    void WorkerNode::start() {
        recorder->readPodRecord();
        rpc_server->startService();
    }
}