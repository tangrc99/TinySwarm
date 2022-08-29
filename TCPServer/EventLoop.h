//
// Created by trc on 2021/10/19.
//

#ifndef TCPTEST_EVENTLOOP_H
#define TCPTEST_EVENTLOOP_H

#include <mutex>
#include <thread>
#include "Timer.h"

class Poller;
class Channel;

static void emptyFunctor(){}

class EventLoop {
private:
    bool looping = false, quit = false;
    Poller *poller;
    std::vector<Channel *> active_channel;
    std::thread::id tid;

    TimerQueue timerQueue;

    typedef std::function<void()> Functor;
    typedef std::vector<Functor> FunctorList;

    std::mutex mtx;
    FunctorList functors,loop_functors;


public:


    EventLoop();

    EventLoop(FunctorList loopFunctors);


    /// 这两个函数之后被初始化的时候使用
    void setPoller(Poller *poller_) ;

    void setTid() {
        tid = std::this_thread::get_id();
    }

    bool isInCurrentThread() {
        return tid == std::this_thread::get_id();
    }

    void loop();

    void doFunctors();

    /// 延迟运行
    void putInFunctors(Functor fun){
        LOG_WARN("%s - %x","Loop has New Functor",this)
        std::lock_guard<std::mutex> gd(mtx);
        functors.push_back(std::move(fun));
    }

    /// 尝试运行
    void RunInLoop(Functor fun){
        if(isInCurrentThread())
            fun();
        else {
            std::lock_guard<std::mutex> gd(mtx);
            functors.push_back(std::move(fun));
        }
    }

    void addChannelToPoller(Channel *new_channel);

    void enableChannelWrite(Channel *channel);

    void enableChannelRead(Channel *channel);

    void removeChannel(Channel *channel) ;


    int removeTimer(const int &id){
        return timerQueue.removeTimer(id);
    }

    std::pair<int,TimerPtr> getTimer(){
        return timerQueue.getTimer();
    }

    ~EventLoop();
};


#endif //TCPTEST_EVENTLOOP_H
