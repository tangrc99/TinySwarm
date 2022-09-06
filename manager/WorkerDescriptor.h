//
// Created by 唐仁初 on 2022/8/2.
//

#ifndef TINYSWARM_WORKERDESCRIPTOR_H
#define TINYSWARM_WORKERDESCRIPTOR_H

#include "Session.h"
#include "PodDescriptor.h"

struct WorkerDescriptor {
    std::string ip;
    int port;

    bool alive;     //FIXME 目前只有check 时会改变 alive 信息

    std::shared_ptr<Session> session;   // 用于通信的连接

    std::list<std::string> services;    // 节点上提供的服务
    std::list<PodDescriptor *> pods; // 当前节点上运行的pod



    void removeService(PodDescriptor *service) {
        for (auto it = pods.begin(); it != pods.end();) {
            if (*it == service) {
                pods.erase(it);
                return;
            } else {
                it++;
            }
        }
    }

    bool portAvailable(int p){
        for(auto &pod : pods){
            if(pod->port_ == p)
                return false;
        }
        return true;
    }
};

#endif //TINYSWARM_WORKERDESCRIPTOR_H
