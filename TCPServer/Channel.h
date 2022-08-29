//
// Created by trc on 2021/10/19.
//

#ifndef TCPTEST_CHANNEL_H
#define TCPTEST_CHANNEL_H

#include "../Common/socket.h"
#include "../Common/IPAddress.h"
#include <iostream>
#include <functional>
#include <utility>
#include <poll.h>
#include <memory>
#include <chrono>
#include <cassert>
#include <sys/socket.h>

#ifdef __linux__
#include <sys/epoll.h>
#endif

enum ConnectionError{
    POLLER_ERR,TIMEOUT
};
/// 实际上 Channel 类对应的是 Poll 所需要的 pollfd


class EventLoop;
class Timer;

class Channel {
private:
    EventLoop *own_loop;
    Socket sockfd;
    IPAddress ipAddress;
    int event = 0;
    int revent = 0;   /// 如果使用的是epoll，则不需要这个参数

    bool isTimer;
    Channel *timer_ch;

    std::shared_ptr<Timer> timer;
    int timer_id;

    ConnectionError error;

    bool tied = false;  /// 表明channel有没有被正确绑定

    typedef std::function<void()> CallBack;
    CallBack readCallBack, writeCallBack, closeCallBack,errorCallBack;

//    typedef std::function<void(ConnectionError)> ErrorCallBack;
//    ErrorCallBack errorCallBack;

public:

    explicit Channel(EventLoop *own_loop,int fd);

    int getFd() { return sockfd.getFd(); }

    void handleEvent();



    void tie() {
        tied = true;
    }

    bool isInTiedThread();

    void updateChannelList();

    void setEvent(int event_) {
        event = event_;
    }

    short getEvent()const{return event;}

    void setRevent(const int &revent_) {
        revent = revent_;
    }

    void setReadCallBack(CallBack rd_cb) {
        readCallBack = std::move(rd_cb);
    }

    void setWriteCallBack(CallBack wt_cb) {
        writeCallBack = std::move(wt_cb);
    }

    void setErrorCallBack(CallBack err_cb) {
        errorCallBack = std::move(err_cb);
    }

    void setCloseCallBack(CallBack cl_cb) {
        closeCallBack = std::move(cl_cb);
    }

    void destroy();

    void enableWrite();

    void enableRead();

    void putInLoop(std::function<void()> fun);

    void setAliveTime(const std::chrono::milliseconds &ms);

    void setAliveTime(int ms){
        setAliveTime(std::chrono::milliseconds(ms));
    }

    void TimeOut();

    void cancelTimer();

    [[nodiscard]] ConnectionError getConnectionError() const ;

    void TimerDestroyCallBack();

    std::string getAddress(){
        return isTimer ? "" : ipAddress.getAddress();
    }

    ~Channel();
};


#endif //TCPTEST_CHANNEL_H
