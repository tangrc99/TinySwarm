//
// Created by 唐仁初 on 2022/9/2.
//

#ifndef TINYSWARM_SCHEDULER_H
#define TINYSWARM_SCHEDULER_H

#include "WorkerDescriptor.h"

class Scheduler {
public:

    explicit Scheduler(const std::list<WorkerDescriptor *> &workers) : workers_(workers) {}

    virtual WorkerDescriptor *getBestWorker(PodDescriptor *pod) = 0;

protected:
    const std::list<WorkerDescriptor *> &workers_;  // reference of manger's workers
};


class RoundRobin final : Scheduler {
public:

    [[nodiscard]] WorkerDescriptor *getBestWorker(PodDescriptor *pod) override {

        int i = 0;

        for(auto &worker : workers_){

            // 检查 worker 的服务和端口是否符合
            if(worker->alive && worker->portAvailable(pod->port_))
                return worker;

        }

        return nullptr;
    }

private:
    int last = 0;
};


#endif //TINYSWARM_SCHEDULER_H
