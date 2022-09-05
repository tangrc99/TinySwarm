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

    using Json = nlohmann::json;

/// 负责处理与用户之间的逻辑，防止都写在manager里面
    class ServiceManager {
    public:

        using Token = std::string;

        explicit ServiceManager(Manager *manager, Proxy *proxy) : manager_(manager), proxy_(proxy) {}


        int createServiceGroup(std::string token, int num, const std::string &service,
                               ServiceType type, int port, const std::vector<char *> &exe_params,
                               const std::vector<char *> &docker_params, int restart = 0) {

            if (token.empty())
                token = generateToken(9);

            PodGroup group;

            for (int i = 0; i < num; i++) {

                auto alias = token;
                if (num > 1)
                    alias += "_" + std::to_string(i);

                auto[res_, error] = manager_->selectWorkerToCreatePod(service, alias, type, port, exe_params,
                                                                      docker_params,
                                                                      restart);

                if (res_) {
                    auto serv = manager_->getServiceInfoIterByAlias(error);
                    group.emplace_back(serv);
                }

            }

            // 创建其代理地址
            auto addr = proxy_->insertAddressPool(token, getPodGroupAddressPool(group));

            ServiceDescriptor sd(token, group, num, addr);


            if (!group.empty()) {
                auto ptr = &map_.emplace(token, sd).first->second;
                //FIXME: 这里如果一个都没有创建完成，那么则返回用户失败，若有创建完成的，则暂时不返回失败，尝试继续创建
                if (group.size() < num)
                    start_list_.emplace_back(ptr);
            }

            return static_cast<int>(group.size());
        }

        bool deleteServiceGroup(const std::string &token) {

            auto group = getPodsInService(token);

            for (auto &pod: group) {
                manager_->stopService(*pod);
            }

            return proxy_->deleteAddressPool(token);
        }

        PodGroup getPodsInService(const std::string &token) {
            auto sd = getServiceDescriptor(token);
            return sd == nullptr ? PodGroup{} : sd->pods_;
        }

        manager::ServiceDescriptor *getServiceDescriptor(const std::string &token) {
            auto it = map_.find(token);
            return it == map_.end() ? nullptr : (&it->second);
        }


    private:

        Token generateToken(int length) {
            char tmp;
            std::string str;

            std::random_device rd;
            std::default_random_engine random(rd());

            for (int i = 0; i < length; i++) {
                tmp = static_cast<char>(random() % 36);
                if (tmp < 10)
                    tmp += '0';
                else
                    tmp += ('A' - 10);
                str += tmp;
            }

            if (map_.find(str) != map_.end())
                return generateToken(length);

            return str;
        }

        // 用来启动未成功的pod
        void startFailedPod() {

        }

        // 用来关闭未成功的pod
        void shutdownFailedPod() {

        }

    private:

        std::unordered_map<Token, manager::ServiceDescriptor> map_;   /// Pod 对象是由 manager 来管理的，只有用户要求关闭服务时才会析构

        std::list<manager::ServiceDescriptor *> start_list_;  // 用来解决用户开启多个pod可能部分失败的问题
        std::list<manager::ServiceDescriptor *> end_list_;  // 用来解决用户关闭多个pod可能失败的问题



        Manager *manager_;
        Proxy *proxy_;
    };
}


#endif //TINYSWARM_SERVICEMANAGER_H
