//
// Created by 唐仁初 on 2022/7/27.
//

#ifndef TINYSWARM_WORKERNODE_H
#define TINYSWARM_WORKERNODE_H


#include "PodDescriptor.h"
#include "RPCServer.h"
#include "WorkerStatus.h"
#include "RPCInterface.h"
#include "ManagerDescriptor.h"
#include "PodRecorder.h"
#include "WorkerError.h"

#include "LiveProbe.h"

#include <list>
#include <iostream>
#include <unistd.h>
#include <filesystem>
#include <sys/fcntl.h>
#include <fstream>
#include <csignal>


namespace worker {

    struct ExitInformation {
        pid_t pid = 0;  //
        int exit_code = 0;
    };

    static std::mutex infos_mtx;
    static std::list<ExitInformation> infos;

/// Receive the SIGCHLD and collect exit information.
    static void childExitHandler(int) {

        int status;
        pid_t pid = wait(&status);

        std::lock_guard<std::mutex> lg(infos_mtx);
        infos.emplace_back(ExitInformation{pid, status});
    }


    class WorkerNode {
    public:

        explicit WorkerNode(const std::string &work_dir) : work_dir_(work_dir), probe_(new TCPProbe) {

            if (!std::filesystem::exists(work_dir)) {
                std::cerr << "Work Directory Not Exits." << std::endl;
                exit(1);
            }

            if (work_dir.back() == '/') {
                status = new WorkerStatus(work_dir, work_dir + "WorkerInformation");
                recorder = std::make_unique<PodRecorder>(work_dir + "ServiceRecord", rpc_service);
            } else {
                status = new WorkerStatus(work_dir, work_dir + "/WorkerInformation");
                recorder = std::make_unique<PodRecorder>(work_dir + "/ServiceRecord", rpc_service);
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


        ~WorkerNode() {

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

        /// Worker RPC Interface. Create a service using fork, and manage it.
        /// \param service the name of service
        /// \param alias the id of service
        /// \param manager usr that create the service
        /// \param type 0 host service, 1 docker service
        /// \param exe_params the params of service
        /// \param docker_params the params of docker container
        /// \param error_text error while creating service
        /// \return pid of the service
        pid_t createProcess(const std::string &service, const std::string &alias, const std::string &manager, int type,
                            int port, std::vector<char *> exe_params,
                            std::vector<char *> docker_params, std::string &error_text, int restart = 0) {

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


        /// Inner function. Only used to restart process that requires start.
        /// \param pd
        /// \param error_text
        /// \return
        pid_t createProcess(const PodDescriptor &pd, std::string &error_text) {
            return createProcess(pd.service, pd.alias, pd.manager, pd.type, pd.port_, pd.exe_params, pd.docker_param,
                                 error_text,
                                 pd.restart);
        }

        /// Inner function. To Force Stop a Host Service Or Docker Service By PodDescriptor.
        /// Only used to stop manager's pods.
        /// \param pd The ServiceDescriptor of Service to be stopped.
        /// \return Is Service Stopped
        bool stopPod(PodDescriptor *pd) {
            return pd->type == 0 ? stopHostPod(pd->pid) : stopDockerPod(pd->pid);
        }

        /// To Force Stop a Service By Pid_t
        /// \param sd The Pid_t of Service to be stopped.
        /// \return Is Service Stopped
        bool stopHostPod(pid_t pid) {

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

        /// Worker RPC Interface. To Force Stop a Service By Alias and Manager.
        /// \param alias The alias of Service to be stopped.
        /// \param manager The manger of Service to be stopped.
        /// \return 0 success, 1 manger error, -1 kill error, -2 alias not found.

        int stopPod(const std::string &alias, const std::string &manager) {
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

        /// To Force Stop a Docker Service By Pid_t
        /// \param pid The ServiceDescriptor of Service to be stopped.
        /// \return Is Service Stopped
        bool stopDockerPod(pid_t pid) {

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

        /// Worker RPC Interface.To Force Stop a Docker Service By Alias and Manager
        /// \param alias The alias of Service to be stopped.
        /// \param manager The manger of Service to be stopped.
        /// \return 0 success, 1 manger error, -1 kill error, -2 alias not found.
        int stopDockerPod(const std::string &alias, const std::string &manager) {

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

        bool checkIfAliasOK(const std::string &manager, const std::string &alias, std::string &reason) {
            for (auto &pod: pods) {
                if (pod.alias == alias) {
                    reason = (pod.manager == manager) ? SAME_SERV : SAME_ALIAS;
                    return false;
                }
            }
            return true;
        }

        /// Check services. If service is down, collect the reason and put it in down_services.
        /// If the service is marked restart, try to restart it.
        /// Only the Process that exits exceptionally will be collect reason.
        void checkIfPodsRunning() {

            if (infos.empty())
                return;

            std::list<ExitInformation> error_infos;
            {
                std::lock_guard<std::mutex> lg(infos_mtx);
                std::swap(infos, error_infos);
            }

            for (auto &info: error_infos) {
                std::lock_guard<std::mutex> lg(mtx1);

                for (auto pod = pods.begin(); pod != pods.end();) {

                    if (pod->pid == info.pid) {

                        // 删除掉ManagerDescriptor中的记录
                        auto md = getOrAddManager(pod->manager);
                        deletePodInManagerDescriptor(md, info.pid);

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
                                                       info.exit_code});

                        } else {
                            // 对于 docker 进程
                            //FIXME : 这里用一个 shell 脚本来解决


                        }

                        std::cout << "erased " << info.pid << " status " << info.exit_code << std::endl;
                        pods.erase(pod);
                        break;

                    } else {
                        pod++;
                    }
                }
            }
        }

        /// Check services. If pod cannot response the live probe, pod will be set down.
        void checkPodsUsingProbe() {
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
                            PodExitInformation(pod->alias, pod->manager, pod->out_file, -1, "BLOCKED"));
                    auto pid = pod->pid;
                    pods.erase(pod);
                    kill(pid, SIGINT);
                }
            }
        }

        /// Worker RPC Interface. Collect The Exit Information and Try to restart Process that exits because of worker down.
        /// \param manager The manager of services.
        /// \param exits The Exit Information.
        void collectExitInformation(const std::string &manager, std::list<PodExitInformation> &exits) {

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
        };

        /// Clean DumpFiles and Start Information of Checked Process
        /// \param exits The ServiceExitInformation of Processes.
        void cleanDumpFiles(const std::list<PodExitInformation> &exits) {
            for (const auto &e: exits) {
                std::filesystem::remove(e.out_file);    // 删除进程的输出文件
                recorder->removePodRecord(e.alias);    // 删除进程的启动信息
            }
        }

        /// Record service startup information. Used By Rpc Handler Class.
        /// \param request The request of rpc.
        void addPodRecord(const ::ForkInput *request) {
            recorder->addPodRecord(request);
        }

        void updateStatus() {
            status->updateStatus();
        }

        auto getStatus(const std::string &key) {
            return status->getStatus(key);
        }

        /// Check if service is available in this worker node.
        /// \param service The service type.
        /// \return True, if available. False, else
        [[nodiscard]] bool isServiceExist(const std::string &service) const {
            return status->isServiceExist(service);
        }

        /// Worker RPC Interface. Check whether a manager is down. If manager is connected or not connected, it is described NOT DOWN.
        /// \param manager The name of manager
        /// \return True, if manager is down. False, else
        bool isManagerDown(const std::string &manager) {
            auto it = managers.find(manager);

            if (it == managers.end())
                return false;

            auto res = it->second.status != 0;
            it->second.status = 0;

            return res;
        }

        void updateManagerTimestamp(const std::string &manager) {
            auto it = managers.find(manager);
            if (it == managers.end())
                return;

            it->second.timestamp = time(nullptr);
            it->second.status = 0;  // 这代表manager重新被激活了
        }

        /// Get a ptr of ManagerDescriptor and update its timestamp.If not exist, such ManagerDescriptor will be added to manages.
        /// \param manager The name of manager.
        /// \return Ptr of ManagerDescriptor of manager.
        ManagerDescriptor *getOrAddManager(const std::string &manager) {
            auto it = managers.find(manager);
            if (it == managers.end()) {
                auto res = managers.emplace(manager, ManagerDescriptor(manager));
                return &res.first->second;
            }

            it->second.timestamp = time(nullptr);
            it->second.status = 0;  // 这代表manager重新被激活了

            return &it->second;
        }

        static bool deletePodInManagerDescriptor(ManagerDescriptor *md, pid_t pid) {
            for (auto pod = md->pods.begin(); pod != md->pods.end(); pod++) {
                if ((*pod)->pid == pid) {
                    md->pods.erase(pod);
                    return true;
                }
            }
            return false;
        }

        /// Delete Only One ManagerDescriptor That Long Not Used. If expire time < 0, the function never runs.
        /// \param time_s Expire Time.Second.
        void removeManagerNotUsed(int time_s) {

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

        void start() {
            recorder->readPodRecord();
            rpc_server->startService();
        }

    private:
        std::list<PodDescriptor> pods;        //FIXME :  加锁后性能可能较差

        std::mutex mtx1, mtx2;
        std::list<PodExitInformation> down_pods;    // 因故退出的，错误信息会收集在这里


        std::list<std::string> unused_files;    // 记录已退出进程的未被清除文件名。

        std::filesystem::path work_dir_;    // 当前节点的工作目录

        RPCServer *rpc_server;  // 当前节点工作的 rpc 服务器
        RPCInterface *rpc_service;  // 当前节点所运行的 rpc 服务

        WorkerStatus *status;   // 当前节点工作状态的管理器

        using Manager = std::string;
        std::map<Manager, ManagerDescriptor> managers;   // manager信息

        std::unique_ptr<PodRecorder> recorder;

        LiveProbe *probe_;  // 用于存活探测
    };

}
#endif //TINYSWARM_WORKERNODE_H
