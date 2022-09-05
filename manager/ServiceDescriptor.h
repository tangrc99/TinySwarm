//
// Created by 唐仁初 on 2022/9/2.
//

#ifndef TINYSWARM_SERVICEDESCRIPTOR_H
#define TINYSWARM_SERVICEDESCRIPTOR_H

#include "PodDescriptor.h"
#include <utility>
#include <vector>


namespace manager {

    using PodGroup = std::vector<PodDescriptor *>;

    Proxy::AddressPool getPodGroupAddressPool(const PodGroup &group) {
        Proxy::AddressPool pool;
        pool.reserve(group.size());

        for (auto &pod: group) {
            pool.emplace_back(pod->wd_->ip + ":" + std::to_string(pod->port_));
        }

        return pool;
    }

    struct ServiceDescriptor {

        std::string token_;  // 用户service的唯一标识
        PodGroup pods_;  // 用户service所启动的pod
        int num_;    // 用户预期的启动数量

        std::string proxy_address_; // 用户连接发送到这里，然后由代理进行转发

        int status_{}; // 这个是用来处理 service 的特殊状态的量，例如一个服务需要多个 Pod，这些pod可能创建时会失败一部分，或者关闭时会失败一部分

        //bool transfer_{}; //是否允许迁移pod，这个transfer的逻辑可能要修改

        ServiceDescriptor(std::string token, PodGroup pods, int num, std::string proxy_address) : token_(std::move(token)),
                                                                                                  pods_(std::move(pods)),
                                                                                                  num_(num),
                                                                                                  proxy_address_(std::move(
                                                                                                          proxy_address)) {}

    };

} // namespace manager

#endif //TINYSWARM_SERVICEDESCRIPTOR_H
