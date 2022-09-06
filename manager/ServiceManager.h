//
// Created by 唐仁初 on 2022/8/29.
//

#ifndef TINYSWARM_SERVICEMANAGER_H
#define TINYSWARM_SERVICEMANAGER_H

#include "Proxy/Proxy.h"
#include "ServiceDescriptor.h"
#include "Manager.h"
#include <vector>
#include <random>
#include <nlohmann/json.hpp>


namespace manager {

/// 负责处理与用户之间的逻辑，防止都写在manager里面，此处只留给用户文本信息，而不留指向内部的指针等
    class ServiceManager {
    public:

        using Token = std::string;

        explicit ServiceManager(Manager *manager, Proxy *proxy) : manager_(manager), proxy_(proxy) {}


        Json createService(std::string token, int num, const std::string &service,
                          ServiceType type, int port, const std::vector<char *> &exe_params,
                          const std::vector<char *> &docker_params, int restart = 0) {

            if (token.empty())
                token = generateToken(9);

            PodGroup group;

            for (int i = 0; i < num; i++) {

                auto alias = token;
                if (num > 1)
                    alias += "_" + std::to_string(i);

                auto res = manager_->selectWorkerToCreatePod(service, alias, type, port, exe_params,
                                                             docker_params,
                                                             restart);

                if (!res.isFail()) {
                    group.emplace_back(res.podDescriptor());
                }
            }

            // 创建其代理地址
            auto addr = proxy_->insertAddressPool(token, getPodGroupAddressPool(group));


            ServiceDescriptor sd(token, group, num, addr, manager_);


            if (!group.empty()) {
                auto ptr = &map_.emplace(token, sd).first->second;
                //FIXME: 这里如果一个都没有创建完成，那么则返回用户失败，若有创建完成的，则暂时不返回失败，尝试继续创建
                if (group.size() < num)
                    start_list_.emplace_back(ptr);
            }

            return sd.toJson();
        }

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

        Json getServiceInformation(const std::string &token){

            auto sd = getServiceDescriptor(token);

            if(sd == nullptr)
                return {};

            return sd->toJson();
        }

        Json getAccessAddress(const std::string &token){

            auto sd = getServiceDescriptor(token);

            if(sd == nullptr)
                return {};

            return sd->getAccessAddress();
        }

    private:

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


        Manager *manager_;
        Proxy *proxy_;

    };
}


#endif //TINYSWARM_SERVICEMANAGER_H
