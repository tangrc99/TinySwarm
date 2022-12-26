//
// Created by 唐仁初 on 2022/8/29.
//

#ifndef TINYSWARM_SERVICEMANAGER_H
#define TINYSWARM_SERVICEMANAGER_H

#include "Proxy/Proxy.h"
#include "Proxy/NoProxy.h"
#include "ServiceDescriptor.h"
#include "RPCInterface.h"
#include "Manager.h"
#include <vector>
#include <random>
#include <nlohmann/json.hpp>


namespace manager {

/// 负责处理与用户之间的逻辑，防止都写在manager里面，此处只留给用户文本信息，而不留指向内部的指针等
    class ServiceManager {
    public:

        using Token = std::string;

        explicit ServiceManager(IPAddress address) : manager_(std::make_unique<Manager>("trc")),
                                                     proxy_(std::make_unique<proxy::NoProxy>()) {

            std::vector<std::function<void()>> functors = {
                    [this] {
                        startFailedPod();
                        shutdownFailedPod();
                    },
                    [this] {
                        manager_->checkPodsOnAllWorker(10);
                        manager_->transferDownPod();
                    }
            };

            server_ = std::make_unique<RPCServer>(address, functors, 1, -1);
            service_ = new RPCInterface(this);
            server_->addService(service_);
        }

        ~ServiceManager() {
            delete service_;
        }

        void run() const {
            server_->startService();
        }

        /// Rpc Interface. Handle client's create request.
        /// \param token Unique service token
        /// \param num Nums to create
        /// \param service Service name to create
        /// \param type Service type to create
        /// \param port Service listen port requested
        /// \param exe_params Service startup params
        /// \param docker_params Service docker startup params
        /// \param restart Is service need restart
        /// \return Create Information in json format
        Json createService(std::string token, int num, const std::string &service,
                           ServiceType type, int port, const std::vector<char *> &exe_params,
                           const std::vector<char *> &docker_params, int restart = 0) {

            // 如果没有指定名称，则需要随机指定
            if (token.empty())
                token = generateToken(9);

            PodGroup group;

            // 创建服务所指定数量的pod，pod名称为 service_xx
            for (int i = 0; i < num; i++) {

                auto alias = token;
                if (num > 1)
                    alias += "_" + std::to_string(i);

                auto res = manager_->selectWorkerToCreatePod(service, alias, type, port, exe_params, docker_params,
                                                             restart);

                if (!res.isFail()) {
                    group.emplace_back(res.podDescriptor());
                }
            }

            if (group.empty())
                return {};

            std::string addr;

            if (port > 0) {
                // 创建其代理地址
                addr = proxy_->insertAddressPool(token, getPodGroupAddressPool(group));
            }

            ServiceDescriptor sd(token, group, num, addr, manager_.get());

            auto res = map_.emplace(token, sd).first;

            manager::ServiceDescriptor *sd_ptr = &res->second;

            if (group.size() < num)
                start_list_.emplace_back(sd_ptr);

            return sd.toJson();
        }

        /// Rpc Interface. Delete a service by its token.
        /// \param token Unique service token
        /// \return Is delete operation ok
        bool deleteService(const std::string &token) {

            auto sd = getServiceDescriptor(token);

            if (sd == nullptr)
                return false;

            sd->stopPodsInService();

            if (sd->status != ServiceStatus::close) {
                end_list_.emplace_back(sd);
            } else {
                removeService(sd);
            }

            // 无论关闭的状态是什么，都要关闭在代理处的注册
            return proxy_->deleteAddressPool(token);
        }

        /// Rpc Interface. Query service detailed information by its token.
        /// \param token Unique service token
        /// \return Detailed information in json format
        Json getServiceInformation(const std::string &token) {

            auto sd = getServiceDescriptor(token);

            if (sd == nullptr)
                return {};

            return sd->toJson();
        }

        /// Rpc Interface. Query service's current host:port by its token.
        /// \param token Unique service token
        /// \return Detailed information in json format
        Json getAccessAddress(const std::string &token) {

            auto sd = getServiceDescriptor(token);

            if (sd == nullptr)
                return {};

            return sd->getAccessAddress();
        }

        /// List current alive worker nodes.
        /// \return Worker list in json format
        Json showWorkerNodes() {
            return manager_->showWorkerNodes();
        }

        /// Connect to a worker with given address.
        /// \param address Worker address
        void connectToWorker(const IPAddress &address) {
            manager_->connectToWorker(address);
        }

        /// If a pod is scheduler by class Manager. Manager impl will call this function to notify network gateway's update.
        /// \param pod_name Name of pod has been revised.
        void schedulerNotifyCallback(const std::string &pod_name);

    private:

        /// Wrapper function. Used to simplify search service group.
        /// \param token Unique service token
        /// \return If token exists, returns ptr pointed to such group. If token don't exists,returns nullptr
        manager::ServiceDescriptor *getServiceDescriptor(const std::string &token) {
            auto it = map_.find(token);
            return it == map_.end() ? nullptr : (&it->second);
        }

        /// Generate a token contains num and alpha. Guarantee the token is different from tokens in map_
        /// \param length The length of token
        /// \return Generated token
        Token generateToken(int length);

        /// Try to create new pods in the first service. If failed, the service will be move to the last.
        void startFailedPod();

        // 用来关闭未成功的pod
        void shutdownFailedPod();

        /// Inner Interface. Used to remove ServiceDescriptor safely.
        void removeService(manager::ServiceDescriptor *sd);

    private:

        std::unordered_map<Token, manager::ServiceDescriptor> map_;   /// Pod 对象是由 manager 来管理的，只有用户要求关闭服务时才会析构

        std::list<manager::ServiceDescriptor *> start_list_;  // 用来解决用户开启多个pod可能部分失败的问题
        std::list<manager::ServiceDescriptor *> end_list_;  // 用来解决用户关闭多个pod可能失败的问题


        RPCInterface *service_;

        std::unique_ptr<Manager> manager_;
        std::unique_ptr<proxy::Proxy> proxy_;  // 用于提供服务网关的支持
        std::unique_ptr<RPCServer> server_;
    };


}// namespace manager


#endif //TINYSWARM_SERVICEMANAGER_H
