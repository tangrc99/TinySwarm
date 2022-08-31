//
// Created by 唐仁初 on 2022/8/29.
//

#ifndef TINYSWARM_MRMANAGER_H
#define TINYSWARM_MRMANAGER_H

#include "ServiceImplInfo.h"
#include <vector>
#include <random>

using MRURI = std::string;
using ServiceGroup = std::vector<ServiceImplInfo *>;

class MRManager {
public:

    MRURI addServiceGroup(const ServiceGroup &group) {
        auto uri = generateMRURI(16);
        map_.emplace(uri, group);
        return uri;
    }

    ServiceGroup getServiceGroup(const MRURI &uri) {
        auto it = map_.find(uri);
        return it == map_.end() ? ServiceGroup() : it->second;
    }


private:

    MRURI generateMRURI(int length) {
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

    std::unordered_map<MRURI, ServiceGroup> map_;


};


#endif //TINYSWARM_MRMANAGER_H
