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
#include <list>
#include <iostream>
#include <unistd.h>
#include <filesystem>
#include <sys/fcntl.h>
#include <fstream>
#include <signal.h>


struct ExitInformation {
    pid_t pid = 0;
    int status = 0;
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

    explicit WorkerNode(const std::string &work_dir)
            : work_dir_(work_dir) {

        if (!std::filesystem::exists(work_dir)) {
            std::cerr << "Work Directory Not Exits." << std::endl;
            exit(1);
        }

        if (work_dir.back() == '/')
            status = new WorkerStatus(work_dir, work_dir + "WorkerInformation");
        else
            status = new WorkerStatus(work_dir, work_dir + "/WorkerInformation");


        // 这里要进行docker的注册，防止后面pull失败

        signal(SIGCHLD, childExitHandler);


        // 设置周期性的任务
        std::vector<std::function<void()>> functors = {
                [this] { checkServices(); },    // 检查进程是否异常退出
                [this] { updateStatus(); },     // 更新 worker 节点信息
                [this] { removeManagerNotUsed(5); }    // 定期移除过期 manager
        };


        rpc_service = new RPCInterface(this);
        rpc_server = new RPCServer(IPAddress(AF_INET, 8989), functors);
        rpc_server->addService(rpc_service);

    }

    ~WorkerNode() {
        // 退出时关闭所有子进程
        for (auto &service: services) {
            kill(service.pid, SIGKILL);
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
                        std::vector<char *> exe_params,
                        std::vector<char *> docker_params, std::string &error_text, int restart = 0) {

        // 打开一个 docker 类型的服务
        if (type == 1) {

            auto out_file = work_dir_;
            out_file.append(service).append(
                    manager + "_" + std::to_string(time(nullptr)) + std::to_string(services.size()));

            // 如果没有指定容器名称，那么本地需要指定一个容器名用于管理
            std::string container_name;

            for (auto it = docker_params.begin(); it != docker_params.end(); it++) {
                if (strcmp(*it, "--name") == 0) {
                    container_name = *(++it);
                    break;
                }
            }

            std::vector<char *> docker_command = {"/usr/local/bin/docker", "run"};
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
                services.emplace_back(
                        PodDescriptor{pid, type, service, alias, container_name, manager, out_file, exe_params,
                                          docker_params, restart});

                std::cout << "catch new task\n";

                auto md = getOrAddManager(manager);
                md->services.emplace_back(&services.back());


            }

            return pid;
        }


        // 打开一个 host 类型的服务
        if (type == 0) {
            // 如果这里的exec 不存在，直接返回一个异常
            std::filesystem::path path(work_dir_);
            path.append(service);
            if (!std::filesystem::exists(path)) {
                error_text = "EXEC NOT EXIT.";
                return -1;
            }


            // 这里要完成的工作： 分配一个输出的文件，判断运行方式，

            auto exec = work_dir_;
            exec.append(service).append(service);

            auto out_file = work_dir_;
            out_file.append(service).append(
                    manager + "_" + std::to_string(time(nullptr)) + std::to_string(services.size()));


            pid_t pid = fork();

            if (pid == 0) {

                //exe_params.emplace(exe_params.begin(),const_cast<char*>(out_file.c_str()));
                exe_params.emplace(exe_params.begin(), const_cast<char *>(exec.c_str()));
                //exe_params.emplace(exe_params.begin(),const_cast<char*>("sh"));
                exe_params.emplace_back(nullptr);

                int fd = open(out_file.c_str(), O_CREAT | O_WRONLY, S_IRWXU); // 这里要增加权限，否则fork出的子进程无法拥有文件所有权
                int res1 = dup2(fd, 1);  // 重定向输出
                int res2 = dup2(fd, 2);
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
                auto res = services.emplace_back(
                        PodDescriptor{pid, type, service, alias, exec, manager, out_file, exe_params, docker_params,
                                          restart});

                std::cout << "catch new task\n";

                auto md = getOrAddManager(manager);
                md->services.emplace_back(&services.back());

            }
            return pid;
        }
        return -1;
    }

    pid_t createProcess(const PodDescriptor &sd, std::string &error_text) {
        return createProcess(sd.service, sd.alias, sd.manager, sd.type, sd.exe_params, sd.docker_param, error_text,
                             sd.restart);
    }

    /// To Force Stop a Host Service Or Docker Service By ServiceDescriptor
    /// \param sd The ServiceDescriptor of Service to be stopped.
    /// \return Is Service Stopped
    bool stopService(const PodDescriptor &sd) {
        return sd.type == 0 ? stopService(sd.pid) : stopDockerService(sd.pid);
    }

    /// To Force Stop a Service By Pid_t
    /// \param sd The Pid_t of Service to be stopped.
    /// \return Is Service Stopped
    bool stopService(pid_t pid) {

        std::cout << "ready to shut down service\n";
        std::lock_guard<std::mutex> lg(mtx1);

        for (auto service = services.begin(); service != services.end(); service++) {
            if (service->pid == pid) {
                kill(pid, SIGKILL);

                auto res = std::filesystem::remove(service->out_file);
                if (!res)
                    unused_files.emplace_back(service->out_file);

//                auto md = &managers.find(service->manager)->second;
//                deleteServiceInManagerDescriptor(md, service->pid);

                removeServiceRecord(service->alias);    // 删除启动文件
                services.erase(service);
                return true;
            }
        }
        return false;
    }

    /// Worker RPC Interface. To Force Stop a Service By Alias and Manager.
    /// \param alias The alias of Service to be stopped.
    /// \param manager The manger of Service to be stopped.
    /// \return 0 success, 1 manger error, -1 kill error, -2 alias not found.
    int stopService(const std::string &alias, const std::string &manager) {
        std::lock_guard<std::mutex> lg(mtx1);

        for (auto service = services.begin(); service != services.end(); service++) {
            if (service->alias == alias) {
                if (service->manager != manager)
                    return 1;

                if (-1 == kill(service->pid, SIGKILL))
                    return -1;


                auto md = getOrAddManager(service->manager);    // 清除 ManagerDescriptor
                deleteServiceInManagerDescriptor(md, service->pid);

                removeServiceRecord(alias); // 清除启动信息
                auto res = std::filesystem::remove(service->out_file);  // 清除输出文件
                if (!res)
                    unused_files.emplace_back(service->out_file);

                services.erase(service);
                return 0;
            }
        }
        return -2;
    }

    /// To Force Stop a Docker Service By Pid_t
    /// \param pid The ServiceDescriptor of Service to be stopped.
    /// \return Is Service Stopped
    bool stopDockerService(pid_t pid) {

        std::string command;
        std::lock_guard<std::mutex> lg(mtx1);

        for (auto service = services.begin(); service != services.end(); service++) {
            if (service->pid == pid) {
                command = "docker stop " + service->exec;
                auto res = std::filesystem::remove(service->out_file);
                if (!res)
                    unused_files.emplace_back(service->out_file);

//                auto md = &managers.find(service->manager)->second;
//                deleteServiceInManagerDescriptor(md, service->pid);

                removeServiceRecord(service->alias);    // 删除启动文件
                services.erase(service);
                break;
            }
        }
        system(command.c_str());
        int res = kill(pid, SIGKILL);

        return true;
    }


    /// Worker RPC Interface.To Force Stop a Docker Service By Alias and Manager
    /// \param alias The alias of Service to be stopped.
    /// \param manager The manger of Service to be stopped.
    /// \return 0 success, 1 manger error, -1 kill error, -2 alias not found.
    int stopDockerService(const std::string &alias, const std::string &manager) {

        std::string command;
        std::lock_guard<std::mutex> lg(mtx1);

        for (auto service = services.begin(); service != services.end(); service++) {
            if (service->alias == alias) {
                if (service->manager != manager)
                    return 1;

                command = "docker stop " + service->exec;

                removeServiceRecord(alias); // 清除启动信息
                auto res = std::filesystem::remove(service->out_file);  // 清除输出文件

                if (!res)
                    unused_files.emplace_back(service->out_file);
                services.erase(service);
                system(command.c_str());    //FIXME: 这里不能确保docker被关闭掉的

                int kill_res = kill(service->pid, SIGKILL);

                if (0 == kill_res) {
                    auto md = getOrAddManager(service->manager);    // 清除 ManagerDescriptor
                    deleteServiceInManagerDescriptor(md, service->pid);
                }

                return kill_res;

            }
        }
        return -2;
    }


    /// Check services. If service is down, collect the reason and put it in down_services.
    /// If the service is marked restart, try to restart it.
    /// Only the Process that exits exceptionally will be collect reason.
    void checkServices() {

        if (infos.empty())
            return;

        std::list<ExitInformation> error_infos;
        {
            std::lock_guard<std::mutex> lg(infos_mtx);
            std::swap(infos, error_infos);
        }

        for (auto &info: error_infos) {
            std::lock_guard<std::mutex> lg(mtx1);

            for (auto service = services.begin(); service != services.end();) {

                if (service->pid == info.pid) {

                    // 删除掉ManagerDescriptor中的记录
                    auto md = getOrAddManager(service->manager);
                    deleteServiceInManagerDescriptor(md, info.pid);

                    // 删除对应的 ServiceRecord
                    removeServiceRecord(service->alias);

                    // 重启进程
                    if (service->restart-- > 0) {
                        std::cout << "restart" << std::endl;
                        std::string error;
                        PodDescriptor sd = *service;
                        services.erase(service);
                        createProcess(sd, error);
                        break;
                    }

                    // 不需要重启进程
                    if (service->type == 0) {
                        // 对于 host 进程，直接通过进程代码来判断

                        std::lock_guard<std::mutex> lg1(mtx2);
                        down_services.emplace_back(
                                ServiceExitInformation{service->alias, service->manager, service->out_file,
                                                       info.status});

                    } else {
                        // 对于 docker 进程
                        //FIXME : 这里用一个 shell 脚本来解决


                    }

                    std::cout << "erased " << info.pid << " status " << info.status << std::endl;
                    services.erase(service);
                    break;

                } else {
                    service++;
                }
            }
        }
    }

    /// Try to restart Process that exits because of worker down. If not succeed, gives an error in exits.
    /// \param manager The manager of services.
    /// \param exits The Exit Information.
    void checkServiceRecord(const std::string &manager, std::list<ServiceExitInformation> &exits) {
        for (auto record = records.begin(); record != records.end();) {
            if (record->owner() == manager) {
                ::ForkEcho echo;
                rpc_service->fork({}, &*record, &echo, {});
                if (echo.fail())
                    exits.emplace_back(ServiceExitInformation(record->alias(), record->owner(),
                                                              "/Users/tangrenchu/CLionProjects/TinySwarm/Services/WorkerExit",
                                                              0));
                // FIXME: 这个文件不一定存在，要在启动的时候创建
                records.erase(record);
            } else {
                record++;
            }
        }
    }

    /// Worker RPC Interface. Collect The Exit Information and Try to restart Process that exits because of worker down.
    /// \param manager The manager of services.
    /// \param exits The Exit Information.
    void collectExitInformation(const std::string &manager, std::list<ServiceExitInformation> &exits) {

        auto wd = getOrAddManager(manager); // 更新timestamp

        checkServiceRecord(manager, exits);

        std::lock_guard<std::mutex> lg(mtx2);

        for (auto service = down_services.begin(); service != down_services.end();) {
            if (service->manager == manager) {
                exits.emplace_back(*service);
                service = down_services.erase(service);
            } else {
                service++;
            }
        }
    };

    /// Clean DumpFiles and Start Information of Checked Process
    /// \param exits The ServiceExitInformation of Processes.
    void cleanDumpFiles(const std::list<ServiceExitInformation> &exits) {
        for (const auto &e: exits) {
            std::filesystem::remove(e.out_file);    // 删除进程的输出文件
            std::filesystem::path path = work_dir_;     // 删除进程的启动信息
            path.append("ServiceRecord").append(e.alias);
            std::filesystem::remove(path);
        }
    }

    /// Write Process input to disk. Path: work_dir/ServiceRecord/alias
    /// \param request The process input.
    void addServiceRecord(const ::ForkInput *request) {
        std::filesystem::path path = work_dir_;
        path.append("ServiceRecord").append(request->alias());

        std::ofstream os(path);

        request->SerializeToOstream(&os);
    }

    void removeServiceRecord(const ::ForkInput *request) {
        removeServiceRecord(request->alias());
    }

    /// Delete Process input in disk.
    /// \param alias The alias of process service.
    void removeServiceRecord(const std::string &alias) {
        std::filesystem::path path = work_dir_;
        path.append("ServiceRecord").append(alias);

        std::filesystem::remove(path);
    }

    /// When worker node is up, check the service record.
    void readServiceRecord() {
        std::filesystem::path path = work_dir_;
        path.append("ServiceRecord");

        for (auto &file: std::filesystem::directory_iterator(path)) {
            std::ifstream is(file);
            ForkInput input;
            input.ParseFromIstream(&is);
            records.emplace_back(input);
        }
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

    /// Worker RPC Interface. Check whether a manager is down. If manager is alive or not connected, it is described NOT DOWN.
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

    bool deleteServiceInManagerDescriptor(ManagerDescriptor *md, pid_t pid) {
        for (auto service = md->services.begin(); service != md->services.end(); service++) {
            if ((*service)->pid == pid) {
                md->services.erase(service);
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

                for (auto service: md.services) {
                    // 停止所有的服务
                    std::cout << "calling force stop " << service->pid << std::endl;
                    stopService(*service);
                }
                md.services.clear();

                md.status = -1; // 将 manager 的状态设置为掉线
                return;
            }
        }
    }

    void start() {
        readServiceRecord();
        rpc_server->startService();
    }

private:
    std::list<PodDescriptor> services;        //FIXME :  加锁后性能可能较差

    std::mutex mtx1, mtx2;
    std::list<ServiceExitInformation> down_services;    // 因故退出的，错误信息会收集在这里


    std::list<std::string> unused_files;    // 记录已退出进程的未被清除文件名。

    std::filesystem::path work_dir_;

    std::list<::ForkInput> records; // 重启时，用于读取崩溃前的启动信息

    RPCServer *rpc_server;
    RPCInterface *rpc_service;

    WorkerStatus *status;

    using Manager = std::string;
    std::map<Manager, ManagerDescriptor> managers;   // manager信息
};


#endif //TINYSWARM_WORKERNODE_H
