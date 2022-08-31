//
// Created by 唐仁初 on 2022/8/29.
//

#ifndef TINYSWARM_GROUPMANAGER_H
#define TINYSWARM_GROUPMANAGER_H

#include "ServiceImplInfo.h"
#include "Manager.h"
#include <vector>
#include <random>

using Token = std::string;
using ServiceGroup = std::vector<ServiceImplInfo *>;


/// 负责处理与用户之间的逻辑，防止都写在manager里面
class GroupManager {
public:

    explicit GroupManager(Manager *manager) : manager_(manager){}


    Token addServiceGroup(const ServiceGroup &group) {
        auto uri = generateMRURI(16);
        map_.emplace(uri, group);
        return uri;
    }

    ServiceGroup getServiceGroup(const Token &uri) {
        auto it = map_.find(uri);
        return it == map_.end() ? ServiceGroup() : it->second;
    }

    int createServiceGroup(const std::string &token, int num, const std::string &service,
                           const std::string &alias, ServiceType type, const std::vector<char *> &exe_params,
                           const std::vector<char *> &docker_params, int restart = 0) {

        ServiceGroup group;

        for (int i = 0; i < num; i++) {
            auto wd = manager_->selectWorker();

            auto[res_, alias_] = manager_->createService(wd, service, alias, type, exe_params, docker_params, restart);

            if (res_) {
                auto serv = manager_->getServiceInfoIterByAlias(alias_);
                group.emplace_back(serv);
            }

        }
        map_.emplace(token, group);

        return static_cast<int>(group.size());
    }

    ServiceGroup findServiceGroup(const std::string &token) {
        auto it = map_.find(token);
        return it == map_.end() ? ServiceGroup{} : (it->second);
    }

    std::vector<std::string> findAddressPool(const std::string &token) {
        auto it = map_.find(token);

        if (it == map_.end())
            return {};

        std::vector<std::string> pool;
        for (auto info: it->second) {
            pool.emplace_back(info->wd_->ip + ":" + std::to_string(info->wd_->port));
        }

        return pool;
    }


private:

    Token generateMRURI(int length) {
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
            return generateMRURI(length);

        return str;
    }

    std::unordered_map<Token, ServiceGroup> map_;

    Manager *manager_;
};


#endif //TINYSWARM_GROUPMANAGER_H
