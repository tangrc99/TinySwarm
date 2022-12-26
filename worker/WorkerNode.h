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
#include "worker/recorder/PodStartRecorder.h"
#include "WorkerError.h"
#include "worker/probe/LiveProbe.h"
#include "worker/util/LockFreeQueue.h"

#include <list>
#include <iostream>
#include <unistd.h>
#include <filesystem>
#include <sys/fcntl.h>
#include <fstream>
#include <csignal>




    /// Struct to record process exit information
    struct ExitInformation {
        pid_t pid = 0;  // exited process id
        int exit_code = 0; // exited code
    };

    /// LockFreeQueue to record process exit information
    static LockFreeQueue<ExitInformation> infos;

/// Receive the SIGCHLD and collect exit information.
    static void childExitHandler(int) {

        int status;
        pid_t pid = wait(&status);
        infos.push(ExitInformation{pid, status});   // 将错误信息收集到无锁队列中
    }

namespace tiny_swarm::worker {
    /// Class WorkerNode manages all process started. This class will accpect manager's request to start new process and
    /// monitor their status using signal. Once a process exits with an exit code > 0, it's exit information(include exit
    /// code, signal), stdout will be collected and send to mongodb and it's manager. Every process is also called Pod. And
    /// Every pod is bind to it's manager. If it's manager is down, the pod will be shutdown by worker.
    class WorkerNode {
    public:

        /// Constructor will do following things. 1.Check assigned directory to find available binary file.
        /// 2.Construct WorkerNode's bridged class. 3.Registry signal handler 4.Start inner rpc server.
        /// \param work_dir
        explicit WorkerNode(const std::string &work_dir);


        ~WorkerNode();

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
                            std::vector<char *> docker_params, std::string &error_text, int restart = 0);


        /// Inner function. Only used to restart process that requires start.
        /// \param pd
        /// \param error_text
        /// \return
        pid_t createProcess(const PodDescriptor &pd, std::string &error_text);

        /// Inner function. To Force Stop a Host Service Or Docker Service By PodDescriptor.
        /// Only used to stop manager's pods.
        /// \param pd The ServiceDescriptor of Service to be stopped.
        /// \return Is Service Stopped
        bool stopPod(PodDescriptor *pd);

        /// To Force Stop a Service By Pid_t
        /// \param sd The Pid_t of Service to be stopped.
        /// \return Is Service Stopped
        bool stopHostPod(pid_t pid);

        /// Worker RPC Interface. To Force Stop a Service By Alias and Manager.
        /// \param alias The alias of Service to be stopped.
        /// \param manager The manger of Service to be stopped.
        /// \return 0 success, 1 manger error, -1 kill error, -2 alias not found.

        int stopPod(const std::string &alias, const std::string &manager);

        /// To Force Stop a Docker Service By Pid_t
        /// \param pid The ServiceDescriptor of Service to be stopped.
        /// \return Is Service Stopped
        bool stopDockerPod(pid_t pid);

        /// Worker RPC Interface.To Force Stop a Docker Service By Alias and Manager
        /// \param alias The alias of Service to be stopped.
        /// \param manager The manger of Service to be stopped.
        /// \return 0 success, 1 manger error, -1 kill error, -2 alias not found.
        int stopDockerPod(const std::string &alias, const std::string &manager);

        bool checkIfAliasOK(const std::string &manager, const std::string &alias, std::string &reason);

        /// Check services. If service is down, collect the reason and put it in down_services.
        /// If the service is marked restart, try to restart it.
        /// Only the Process that exits exceptionally will be collect reason.
        void checkIfPodsRunning();

        /// Check services. If pod cannot response the live probe, pod will be set down.
        void checkPodsUsingProbe();

        /// Worker RPC Interface. Collect The Exit Information and Try to restart Process that exits because of worker down.
        /// \param manager The manager of services.
        /// \param exits The Exit Information.
        void collectExitInformation(const std::string &manager, std::list<PodExitInformation> &exits);;

        /// Clean DumpFiles and Start Information of Checked Process
        /// \param exits The ServiceExitInformation of Processes.
        void cleanDumpFiles(const std::list<PodExitInformation> &exits);

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
        bool isManagerDown(const std::string &manager);

        void updateManagerTimestamp(const std::string &manager);

        /// Get a ptr of ManagerDescriptor and update its timestamp.If not exist, such ManagerDescriptor will be added to manages.
        /// \param manager The name of manager.
        /// \return Ptr of ManagerDescriptor of manager.
        ManagerDescriptor *getOrAddManager(const std::string &manager);

        static bool deletePodInManagerDescriptor(ManagerDescriptor *md, pid_t pid);

        /// Delete Only One ManagerDescriptor That Long Not Used. If expire time < 0, the function never runs.
        /// \param time_s Expire Time.Second.
        void removeManagerNotUsed(int time_s);

        void start();

    private:

        std::filesystem::path work_dir_;    // 当前节点的工作目录



        std::list<PodDescriptor> pods;        //FIXME :  加锁后性能可能较差

        std::mutex mtx1, mtx2;
        std::list<PodExitInformation> down_pods;    // 因故退出的，错误信息会收集在这里


        std::list<std::string> unused_files;    // 记录已退出进程的未被清除文件名。

        RPCServer *rpc_server;  // 当前节点工作的 rpc 服务器
        RPCInterface *rpc_service;  // 当前节点所运行的 rpc 服务

        WorkerStatus *status;   // 当前节点工作状态的管理器

        using Manager = std::string;
        std::map<Manager, ManagerDescriptor> managers;   // manager信息

        std::unique_ptr<recorder::PodStartRecorder> recorder;

        LiveProbe *probe_;  // 用于存活探测
    };

}
#endif //TINYSWARM_WORKERNODE_H
