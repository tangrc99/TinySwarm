//
// Created by 唐仁初 on 2022/8/21.
//

#ifndef TINYSWARM_MANAGERDESCRIPTOR_H
#define TINYSWARM_MANAGERDESCRIPTOR_H

#include "PodDescriptor.h"
#include <list>
#include <utility>

namespace tiny_swarm::worker {

    struct ManagerDescriptor {

        std::string usr_info; // manager的名称
        time_t timestamp;   // 上一次产生会话的时间
        int status; // manager 存活状态 0 存活 -1 掉线

        std::list<PodDescriptor *> pods;    // manager启动的 Service


        explicit ManagerDescriptor(std::string manager) : usr_info(std::move(manager)), timestamp(time(nullptr)),
                                                          status(0) {

        }
    };
}


#endif //TINYSWARM_MANAGERDESCRIPTOR_H
