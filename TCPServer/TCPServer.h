//
// Created by trc on 2021/10/19.
//

#ifndef TCPTEST_TCPSERVER_H
#define TCPTEST_TCPSERVER_H

#include <iostream>
#include <set>
#include <memory>
#include "ThreadPool.h"
#include "../Common/IPAddress.h"

//#ifdef __linux__
//#define POLLER_SIGNAL 0
//#else
#define POLLER_SIGNAL 1
//#endif


class Acceptor;
class TCPConnection;




class TCPServer {
private:
    Acceptor *acceptor;
    EventLoop *listen_loop;     /// listen_loop 由用户创建，用户管理周期
    Poller *poller;
    typedef std::set<std::shared_ptr<TCPConnection>> Connections;
    Connections connections;
    int listen_fd;

    ThreadPool *thread_pool;

    IPAddress local_address;

    typedef std::function<void(char *, size_t)> CallBack;
    CallBack readCallBack, errorCallBack;

    typedef std::function<void(int)> CloseCallBack;
    CloseCallBack closeCallBack;

    typedef std::function<void(const std::string &, size_t, std::string&)> WriteCallBack;
    WriteCallBack writeCallBack;

    static Poller *createPoll(const int &listen_fd);


public:

    explicit TCPServer(IPAddress addr, EventLoop *listen_loop, int thread_num, int timeout_ms);

    void setReadCallBack(CallBack cb) {
        readCallBack = std::move(cb);
    }

    void setWriteCallBack(WriteCallBack cb) {
        writeCallBack = std::move(cb);
    }

    void setErrorCallBack(CallBack cb) {
        errorCallBack = std::move(cb);
    }

    void setCloseCallBack(CloseCallBack cb){
        closeCallBack = std::move(cb);
    }

    Channel *newConnection(const int &conn_fd);

    void removeConnection(const std::shared_ptr<TCPConnection> &conn) {

        /// 移出 Connection 的管理权，使其等待析构
        if (connections.find(conn) != connections.end()) {
            connections.erase(conn);
        }
    }

    void startListen();

    ~TCPServer() {
        delete thread_pool;
        delete acceptor;
    }

};

#endif //TCPTEST_TCPSERVER_H
