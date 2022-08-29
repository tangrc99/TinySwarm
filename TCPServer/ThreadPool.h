//
// Created by 唐仁初 on 2021/10/21.
//

#ifndef TCPTEST_THREADPOOL_H
#define TCPTEST_THREADPOOL_H

#include <thread>
#include <vector>
#include <algorithm>
#include <cassert>
#include "EventLoop.h"

#define MAX_THREAD_NUM 15


class ThreadPool {
private:
    std::vector<std::thread> threads;
    EventLoop *current_loop,*main_loop;
    std::vector<EventLoop*> loops{};

    std::function<void()> cb;

    int thread_num;
    bool run = true;

public:
    explicit ThreadPool(int thread_num_,EventLoop *listen_loop);

    EventLoop *getAnEventLoop();

    ~ThreadPool(){
        /// 监听线程不是在这里创建的
        run = false;

        for(int i=1;i<loops.size();i++)
            delete loops[i];
    }
};


#endif //TCPTEST_THREADPOOL_H
