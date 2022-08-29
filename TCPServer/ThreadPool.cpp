//
// Created by 唐仁初 on 2021/10/21.
//

#include "ThreadPool.h"
#include "../logUtils/Logging.h"


ThreadPool::ThreadPool(int thread_num_, EventLoop *listen_loop) :thread_num(thread_num_) {
    assert(thread_num <= MAX_THREAD_NUM);

    if (thread_num > MAX_THREAD_NUM) LOG_FATAL("%s: %d", "THREAD NUM", thread_num)

    /// 主线程是由用户手动启动的 ///
    loops.push_back(listen_loop);
    listen_loop->setTid();
    main_loop = loops[0];
    current_loop = main_loop;
    LOG_INFO("%s:%x , %s:%x", "Main Thread", std::this_thread::get_id(), "Main EventLoop", main_loop)

    for (int i = 1; i < thread_num; i++) {

        loops.push_back(new EventLoop());

        threads.emplace_back([this, i]() {

            auto loop = loops[i];
            loop->setTid();
            LOG_INFO("%s:%x , %s:%x", "Thread", std::this_thread::get_id(), "EventLoop", loops[i])

            while (run) {
                loop->loop();
            }

        });
    }
}

EventLoop *ThreadPool::getAnEventLoop() {
    if (thread_num == 1)
        return main_loop;
    EventLoop *assigned_loop;
    auto pos = std::find(loops.begin(), loops.end(), current_loop);
    if (pos == loops.begin() | pos == loops.end())
        assigned_loop = *(loops.begin() + 1);
    else
        assigned_loop = *(pos + 1);

    LOG_INFO("%s: %x", "Assigned EventLoop:", assigned_loop);

    return assigned_loop;
}