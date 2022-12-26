//
// Created by 唐仁初 on 2022/7/28.
//

#ifndef TINYSWARM_MANAGER_H
#define TINYSWARM_MANAGER_H

#include "Result.h"
#include "WorkerDescriptor.h"
#include "WorkerClient.h"
#include "PodDescriptor.h"
#include "LogQueue.h"
#include "RPCServer.h"
#include "Scheduler/Scheduler.h"
#include "Publisher/Publisher.h"

#include <list>
#include <sstream>
#include <utility>

namespace manager {

    class Manager {

    public:


        explicit Manager(std::string usr) : usr_info(std::move(usr)), cur_line(0),
                                            scheduler_(new scheduler::RoundRobin(workers)),
                                            publisher_(new publisher::RedisPublisher()) {   //FIXME: 此处new的没有删除

            logger = std::make_unique<LogQueue>("./manager.log");
            logger->start();

            // 从snapshot 和 log 中恢复之前的信息
            recover();

            // 创建一个redis实例以及redis的客户端

        }

        ~Manager() {
            if (snapshot_thread.joinable())
                snapshot_thread.join();

            delete scheduler_;
            delete publisher_;
        }


        // 错误处理： 如果是任务出现异常，那么使用check rpc来收集错误信息，并且尝试重启
        //           如果是worker节点出现异常，那么将worker节点上的全部任务调度到其他地方


        /// Select a worker from alive workers and then create pod on it. This function will call function selectWorker and createPod.
        /// \param service The service to create
        /// \param alias The alias of service
        /// \param type The type of service
        /// \param port The port of service
        /// \param exe_params The params of service
        /// \param docker_params The docker start params
        /// \param restart The time service will restart, -1 means whenever
        /// \return [if created, error info]
        CreateResult selectWorkerToCreatePod(const std::string &service, const std::string &alias,
                                             ServiceType type, int port,
                                             const std::vector<char *> &exe_params,
                                             const std::vector<char *> &docker_params, int restart = 0);

        /// RPC, Create a service on a worker node with WorkDescriptor. If succeed, current node
        /// will publish message to other manager nodes.
        /// \param wd WorkerDescriptor of worker node to create service.
        /// \param service The service to create.
        /// \param alias The alias of service.
        /// \param type The type of service.
        /// \param exe_params The params of service.
        /// \param docker_params The docker start params, if service run in docker.
        /// \param restart The time service will restart, -1 means whenever.
        /// \return [ if created, error info ]
        CreateResult createPod(WorkerDescriptor *wd, const std::string &service, const std::string &alias,
                               ServiceType type, int port, const std::vector<char *> &exe_params,
                               const std::vector<char *> &docker_params, int restart = 0);


        /// User Interface, force shutdown a service using ServiceInfo.Atomic Function.
        /// \param info ServiceInfo of service to stop.
        /// \return If Succeed.
        std::pair<bool, std::string> stopPod(PodDescriptor *info);

        ///
        /// \param info
        /// \param new_wd
        /// \return
        /// @note 因为这里涉及到对象的复制，所以用引用更好
        std::pair<bool, std::string> transferPod(PodDescriptor &info, WorkerDescriptor *new_wd);


        /// Communicate withe all workers and check pods' status. If any of worker is unconnected, manager will try to reconnect.
        /// If a pod was exit, the worker will return its exit num, reason and log file.
        /// \param line The num of log file lines worker will returned
        void checkPodsOnAllWorker(int line);


        /// Connect to a new worker with given address. This function will be called when manager is initialized.
        /// \param ipAddress The address of worker
        /// \return The descriptor of worker. If not succeed, the descriptor will be set down. Manager will try to reconnect
        /// periodically
        WorkerDescriptor *connectToWorker(IPAddress ipAddress);


        /// Find WorkerDescriptor using ip and port.
        /// \param ip ip of WorkerDescriptor.
        /// \param port port of WorkerDescriptor.
        /// \return ptr of WorkerDescriptor or nullptr if not found.
        WorkerDescriptor *findWorkerDescriptor(const std::string &ip, int port);


        /// Find a worker descriptor by address with given string.
        /// \param address The address of worker
        /// \return ptr of WorkerDescriptor or nullptr if not found.
        WorkerDescriptor *findWorkerDescriptor(const std::string &address);

        /// Find ServiceInfo using alias.
        /// \param alias alias of service
        /// \return ptr of ServiceInfo or nullptr if not found.
        PodDescriptor *getPodDescriptorByAlias(const std::string &alias);


        /// Delete DownService when it is up again or stopped by user.
        /// \param pod Ptr of DownService.
        void removeDownService(PodDescriptor *pod);

        /// Transfer down pod to another worker
        void transferDownPod();


        /// Get information of all workers. The format is { "workers" : [ { worker1 } ] }
        /// \return The Json format information of workers
        nlohmann::json showWorkerNodes();


        /// Get descriptors of all pods that runs server. This function is only used for debugging.
        /// \param service Service name.
        /// \return Vector of pod descriptors required.
        /// @warning This function is only used for debugging because it is not thread safe
        std::vector<const PodDescriptor *> showServices(const std::string &service);

    private:

        /// Use scheduler module to find a best worker node to create a pod.
        /// \param pod Pod descriptor with all information but worker node
        /// \return Selected worker descriptor
        WorkerDescriptor *selectWorker(PodDescriptor *pod) const {
            return scheduler_->getBestWorker(pod);
        }


        /// RPC, Create a service on a worker node with ServiceInfo. If succeed, current node
        /// will publish message to other manager nodes. The ServiceInfo will be set connected.
        /// \param info ServiceInfo of service to create. Warning This ServiceInfo is in services map.
        /// \return [if created, error info]
        CreateResult createService(PodDescriptor &info);


        /// RPC, Force shutdown a service in worker node.No any other actions.Used by stopPod and transferPod.
        /// \param wd WorkerDescriptor of worker node to shutdown service.
        /// \param service The name of service.
        /// \param alias The alias of service.
        /// \param type The type of service.
        /// \return [if created, error info]
        std::pair<bool, std::string>
        shutdownPod(WorkerDescriptor *wd, std::string service, std::string alias, ServiceType type);


        /// Median Interface, force shutdown a service using ServiceInfo.Atomic Function.
        /// \param info
        /// \return
        std::pair<bool, std::string> shutdownPod(PodDescriptor *info) {
            if (!info->alive_) {
                return {false, "To shutdown a dead service"};
            }
            auto res = shutdownPod(info->wd_, info->service_, info->alias_, info->type_);
            info->alive_ = res.first;
            // info.wd_->removeService(&info);  在最外层调用
            return res;
        }


        /// Collect Error Information If Service Is Down.
        /// \param wd WorkerDescriptor of worker node.
        /// \param line The last lines of output of down service.
        void checkServices(WorkerDescriptor *wd, int line);


        void publishMessage(const PodDescriptor &info) const;


        WorkerDescriptor *connectToWorker(WorkerDescriptor *wd);


        // 由于是一个单线程的事务，所以开始snapshot的时候，其他事务要么失败，要么成功，无中间态
        bool snapshot();


        void snapshotToServiceInfo(const std::string &snapshot_line, PodDescriptor &info);

        void parseLogLine(const std::string &log_line);

        // 这里无论断连的时间是多少，只要重新将服务放回到原来位置上就可以，具体怎么做由worker来决定
        void recover();


    private:

        std::string usr_info;   // 当前manager的用户名

        // log utils
        std::thread snapshot_thread;
        std::unique_ptr<LogQueue> logger;
        int cur_line;

        // worker nodes
        std::list<WorkerDescriptor *> workers;  // 链表比较短，并且用的机会少，所以存活和非存活可以放在一起

        WorkerClient  cli;


        using Alias = std::string;
        std::map<Alias, PodDescriptor> pods;  // 用于 Service 的查找(暴露给用户方便)。 std::map 容器的元素在内存中的位置不会变
        std::list<PodDescriptor *> down_services; // 用于 Manager 内部的重连操作，用户可以定义是否允许迁移等。

        time_t check_time = 0;  // 因为底层 TCP 没加计时任务功能，这个是用于计时的

        scheduler::Scheduler *scheduler_;  // worker节点调度器
        publisher::Publisher *publisher_;  // manager节点信息同步器
    };

}
#endif //TINYSWARM_MANAGER_H
