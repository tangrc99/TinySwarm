//
// Created by trc on 2021/10/19.
//

#ifndef TCPTEST_TCPCONNECTION_H
#define TCPTEST_TCPCONNECTION_H

#include "Channel.h"
#include <iostream>
#include <utility>
#include <cstring>
#include <cassert>
#include <cerrno>
#include "Buffer.h"


static void defaultWriteCallBack(const std::string &in, size_t in_size, std::string &out) {
    out = "server received...";
}

static void defaultCallBack(char *s, size_t t) {
}

static void defaultCloseCallBack(int fd) {

}

class TCPServer;

class TCPConnection : public std::enable_shared_from_this<TCPConnection> {
private:
    Channel *current_channel;   /// 一个connection 和一个 channel 对应
    TCPServer *own_server;

    typedef std::function<void(char *, size_t)> EventCallBack;
    EventCallBack readCallBack, errorCallBack;

    typedef std::function<void(int)> CloseCallBack;
    CloseCallBack closeCallBack;

    typedef std::function<void(const std::string &, size_t, std::string &)> WriteCallBack;
    WriteCallBack writeCallBack;

    Buffer in_buffer;

    bool empty_read = false;

    void closeConnection();

    int zero_num = 0;

public:
    explicit TCPConnection(TCPServer *server, Channel *ch);

    void setReadCallBack(EventCallBack CallBack) {
        readCallBack = std::move(CallBack);
    }

    void setWriteCallBack(WriteCallBack CallBack) {
        writeCallBack = std::move(CallBack);
    }

    void setErrorCallBack(EventCallBack CallBack) {
        errorCallBack = std::move(CallBack);
    }

    void setCloseCallBack(CloseCallBack CallBack) {
        closeCallBack = std::move(CallBack);
    }

    void handleReadEvent();

    void handleWriteEvent();

    void handleCloseEvent();

    void handleErrorEvent();

    ~TCPConnection();

};


#endif //TCPTEST_TCPCONNECTION_H
