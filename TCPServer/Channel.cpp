//
// Created by trc on 2021/10/19.
//

#include "Channel.h"
#include "EventLoop.h"
#include "Timer.h"
#include "Logging.h"
#include <utility>


Channel::Channel(EventLoop *own_loop, int fd) :
        own_loop(own_loop), sockfd(fd), isTimer(false), timer_ch(nullptr),
        timer_id(-1), timer(nullptr) , ipAddress(fd) {

}


//#ifdef __linux__
//void Channel::handleEvent() {
//        cancelTimer();
//
//        //std::cout<<revent;
//        if ((revent & EPOLLHUP) &&!(revent & EPOLLRDNORM)) {
//            std::cout<<"CLOSE EVENT\n";
//            closeCallBack();
//        } else if ((revent & EPOLLERR)) {
//            std::cout<<"ERROR EVENT\n";
//                    error = ConnectionError::POLLER_ERR;
//            errorCallBack();
//        } else if (revent & EPOLLRDNORM) {
//            std::cout<<"READ EVNET\n";
//            readCallBack();
//        } else if (revent & EPOLLWRNORM) {
//            std::cout<<"WRITE EVENT\n";
//            writeCallBack();
//        }else{
//            std::cout<<"OTHER EVENT : "<<revent <<std::endl;
//            exit(-1);
//        }
//    }
//#else

void Channel::handleEvent() {
    cancelTimer();
    LOG_DEBUG("%s: %x", "Channel Handling", this)
    if ((revent & POLLHUP) && !(revent & POLLRDNORM)) {
        LOG_DEBUG("%s:%d - %s", "FD", getFd(), "CLOSE EVENT")
        closeCallBack();
    } else if ((revent & POLLERR)) {
        LOG_DEBUG("%s:%d - %s", "FD", getFd(), "ERROR EVENT")
        error = ConnectionError::POLLER_ERR;
        errorCallBack();
    } else if (revent & POLLRDNORM) {
        LOG_DEBUG("%s:%d - %s", "FD", getFd(), "READ EVENT")
        readCallBack();
    } else if (revent & POLLWRNORM) {
        LOG_DEBUG("%s:%d - %s", "FD", getFd(), "WRITE EVENT")
        writeCallBack();
    } else {
        if (isTimer) {
            readCallBack();
            return;
        }
        LOG_ERROR("%s:%d - %s", "FD", getFd(), "UNKNOWN EVENT")
        LOG_ERROR("%s:%x", "SIGNAL", revent)
        this->setAliveTime(200);
    }
}

//#endif

void Channel::updateChannelList() {
    tie();
    own_loop->addChannelToPoller(this);
}

void Channel::destroy() {
    own_loop->removeChannel(this);
}

void Channel::enableWrite() {
    own_loop->enableChannelWrite(this);
}

void Channel::enableRead(){
    own_loop->enableChannelRead(this);
}

bool Channel::isInTiedThread() {
    return own_loop->isInCurrentThread();
}

void Channel::putInLoop(std::function<void()> fun) {
    own_loop->putInFunctors(std::move(fun));
}

void Channel::cancelTimer() {
    if (timer_ch != nullptr) {
        timer->cancel();
        LOG_DEBUG("%s", "Timer Canceled")
    }
}

/// 只要监听的事件没有销毁定时器（完成任务），定时器每次循环都会是可读的
void Channel::TimeOut() {
    if (timer->isCanceled()) {
        if (!timer->isRunning()) {
            LOG_DEBUG("%s", "TimeOut but Canceled reset timer")

            timer = nullptr;
            //own_loop->removeTimer(timer_id);
            own_loop->putInFunctors([this]{
                own_loop->removeChannel(timer_ch);
                delete timer_ch;
                setAliveTime(std::chrono::milliseconds(5000));
            });

//            auto p = timer; /// 每个循环结束的时候重设定时器，这样处理定时器只需要等待略大于 POLLER 的时间就可以知道事件是否还活跃
//            auto loop = own_loop;
//
//            revent = 0;
//
//            loop->putInFunctors([p]() {
//                LOG_DEBUG("%s", "Reset Timer")
//                p->setTimeOut(std::chrono::milliseconds(5000));
//            });
        }
        return;
    }

    LOG_DEBUG("%s", "Timer timeout")

    /// 去除 Timer自身
    /// 注意，timer_ch 绑定的是其母 ch 的 TimeOut 函数，因此 remove 的是 timer_ch 而不是 this
    //own_loop->removeChannel(timer_ch);  ///并没有被析构
    //own_loop->removeTimer(timer_id);    /// Timer 要被析构


    /// 判断线程是否被设为活跃状态。
    /// 若活跃 revent 标志会被设置
    if (revent <= 0) {
        LOG_WARN("%s", "Timer calling closeCallBack")
        error = ConnectionError::TIMEOUT;
        errorCallBack();
        closeCallBack();   /// 超时未活跃，进行关闭
    }
}


/// 给 Channel 加入生命周期，若超时则会被关闭
void Channel::setAliveTime(const std::chrono::milliseconds &ms) {
    /// 禁止给定时器加定时器的套娃行为
    assert(isTimer == false);

    //auto time_pair = own_loop->getTimer();
    //timer_id = time_pair.first;
    timer = std::make_shared<Timer>() ;

    LOG_DEBUG("%s", "get timer")
    timer_ch = new Channel(own_loop, timer->getNoticeFd());

    timer_ch->setEvent(POLLRDNORM);
    timer_ch->setRevent(0);
    timer_ch->setReadCallBack(std::bind(&Channel::TimeOut, this));
    timer_ch->updateChannelList();
    timer_ch->isTimer = true;
    timer->setTimeOut(ms);

    LOG_WARN("%s:%d,%d", "New timer(fd,timeout)", timer->getNoticeFd(), ms)
}

ConnectionError Channel::getConnectionError() const {
    return error;
}

Channel::~Channel() {
    if (timer_ch != nullptr) {
        LOG_WARN("%s", "Ready to destroy timer_ch")
        if (timer->isRunning()) {
            LOG_WARN("%s", "Timer is running")
            /// 如果定时器没有触发，在这里去除定时器事件
            timer->cancel();
            timer_ch->timer = timer;    /// 移交 timer 的控制权
            timer_ch->timer_id = timer_id;
            timer_ch->setReadCallBack(std::bind(&Channel::TimerDestroyCallBack, timer_ch));

        } else {
            LOG_WARN("%s", "Timer not running")
            //own_loop->removeTimer(timer_id);
            own_loop->removeChannel(timer_ch);
            delete timer_ch;
        }
    }
    LOG_WARN("%s%x", "Channel Destroyed - ", this)
}

void Channel::TimerDestroyCallBack() {
    LOG_WARN("%s", "Timer Destroy CallBack")
    //own_loop->removeTimer(timer_id);
    own_loop->putInFunctors([this]() {
        own_loop->removeChannel(this);
        delete this;
    });
}

//Channel::~Channel() {
//    if (timer_ch != nullptr) {
//        LOG_WARN("%s", "Ready to destroy timer_ch")
//        if (!timer->isTimeOut()) {
//            timer->cancel();
//            timer_ch->destroy();
//            LOG_WARN("%s", "timer_ch and timer to be removed")
//
//        } /// 如果定时器没有触发，在这里去除定时器事件
//
//
//        Channel *son_channel = timer_ch;
//        std::shared_ptr<Timer> timer1 = timer;
//
//
//        own_loop->putInFunctors([son_channel, timer1]() {
//
//            auto loop = son_channel->own_loop;
//            delete son_channel;    /// 延迟析构，防止 timer_ch 已活跃
//            //timer1->destroy();
//
//            if (timer1->isRunning()) {
//                LOG_WARN("%s", "DELAY DESTROY")
//                loop->putInFunctors([loop, timer1]() {
//                    if (timer1->isRunning()) {
//                        LOG_WARN("%s", "WAIT DESTROY")
//                        std::thread wait_destruction([loop, timer1](){
//                            LOG_FATAL("%s","NEW THREAD CREATED")
//                            while(timer1->isRunning()){
//                                std::this_thread::sleep_for(std::chrono::milliseconds(50));
//                            }
//                            /// 因为线程不同，One thread per loop 思想，其他线程只能给 loop 布置任务
//                            LOG_WARN("%s", "ASSIGNED DESTROY")
//                            loop->putInFunctors([loop,timer1](){
//                                loop->removeTimer(timer1->getNoticeFd());
//                            });
//                        });
//                        //wait_destruction.detach();    /// 如果加这句话，析构不会有提示
//                    } else {
//                        loop->removeTimer(timer1->getNoticeFd());
//                    }
//                });
//            } else {
//                loop->removeTimer(timer1->getNoticeFd());
//            }
//        });
//    }
//    LOG_WARN("%s%x", "Channel Destroyed - ", this)
//}

