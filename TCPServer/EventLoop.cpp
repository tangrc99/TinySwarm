//
// Created by trc on 2021/10/19.
//

#include "EventLoop.h"

#include <utility>
#include "Poller.h"
#include "Logging.h"

//#ifdef __linux__
//EventLoop::EventLoop() : poller(new Epoll()) {
//    }
//#else

EventLoop::EventLoop() : poller(new Poll()) {
}

EventLoop::EventLoop(EventLoop::FunctorList loopFunctors) :  poller(new Poll()) ,loop_functors(std::move(loopFunctors)){

}


//#endif

void EventLoop::loop() {

    looping = true;
    while (!quit) {

        active_channel.clear();

        poller->poll(active_channel);
        if (!active_channel.empty()) {
            LOG_DEBUG("%s %x - %d %s", "Loop", this, active_channel.size(), "things happen")

            for (auto channel: active_channel) {
                channel->handleEvent();
            }
            for (auto channel: active_channel) {
                channel->setRevent(0);/// 事件结束后清零，连接阻塞时将被重设 timer 关闭
            }
        }

        for(auto &functor : loop_functors)
            functor();  //每次循环默认的工作

        doFunctors();
    }
    looping = false;
}

void EventLoop::doFunctors() {
    if (functors.empty())
        return;
    LOG_DEBUG("%s", "start doFunctors")
    FunctorList current_functors;
    {
        std::lock_guard<std::mutex> gd(mtx);
        current_functors.swap(functors);
    }
    for (const auto &functor: current_functors){
        if(functor)
            functor();
    }
}


void EventLoop::setPoller(Poller *poller_) {
    delete poller;
    poller = poller_;
}

/// 新建立的 Channel 被分配到 Poller
void EventLoop::addChannelToPoller(Channel *new_channel) {
    if (isInCurrentThread())
        poller->addChannel(new_channel);
    else {
        LOG_DEBUG("%s:%x %s", "Channel", new_channel, "added to another thread")
        std::lock_guard<std::mutex> gd(mtx);
        functors.push_back([this, new_channel]() {
            poller->addChannel(new_channel);
        });
    }
}

void EventLoop::enableChannelWrite(Channel *channel) {
    if (isInCurrentThread())
        poller->enableChannelWrite(channel);
    else {
        std::lock_guard<std::mutex> gd(mtx);
        functors.push_back([this, channel]() {
            poller->enableChannelWrite(channel);
        });
    }
}

void EventLoop::enableChannelRead(Channel *channel) {
    if (isInCurrentThread())
        poller->enableChannelRead(channel);
    else {
        std::lock_guard<std::mutex> gd(mtx);
        functors.push_back([this, channel]() {
            poller->enableChannelRead(channel);
        });
    }
}

void EventLoop::removeChannel(Channel *channel) {
    if (isInCurrentThread())
        poller->removeChannel(channel);
    else {
        std::lock_guard<std::mutex> gd(mtx);
        functors.push_back([this, channel]() {
            poller->removeChannel(channel);
        });
    }
}

EventLoop::~EventLoop() {
    quit = true;
    delete poller;
}

