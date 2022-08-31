//
// Created by 唐仁初 on 2022/8/2.
//

#ifndef TINYSWARM_WORKERDESCRIPTOR_H
#define TINYSWARM_WORKERDESCRIPTOR_H

#include "Session.h"

struct ServiceImplInfo;

struct WorkerDescriptor {
    std::string ip;
    int port;

    bool alive;     //FIXME 目前只有check 时会改变 alive 信息

    std::shared_ptr<Session> session;   // 用于通信的连接

    std::list<ServiceImplInfo *> services; // 当前节点上运行的服务

    void removeService(ServiceImplInfo *service) {
        for (auto it = services.begin(); it != services.end();) {
            if (*it == service) {
                services.erase(it);
                return;
            } else {
                it++;
            }
        }
    }
};

#endif //TINYSWARM_WORKERDESCRIPTOR_H
