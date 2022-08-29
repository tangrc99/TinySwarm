//
// Created by trc on 2021/10/19.
//

#include <sys/stat.h>
#include "../Common/HTTPData.h"
#include "TCPConnection.h"
#include "TCPServer.h"
#include "../logUtils/Logging.h"


TCPConnection::TCPConnection(TCPServer *server, Channel *ch) :

        own_server(server), current_channel(ch) {

    closeCallBack = defaultCloseCallBack;

    ch->setReadCallBack(std::bind(&TCPConnection::handleReadEvent, this));
    ch->setWriteCallBack(std::bind(&TCPConnection::handleWriteEvent, this));
    ch->setErrorCallBack(std::bind(&TCPConnection::handleErrorEvent, this));
    ch->setCloseCallBack(std::bind(&TCPConnection::handleCloseEvent, this));

}

void TCPConnection::closeConnection() {
    /// 等待被析构
    own_server->removeConnection(shared_from_this());
}

TCPConnection::~TCPConnection() {

    current_channel->destroy(); /// 通知当前的 channel 准备关闭
    delete current_channel;
    LOG_WARN("%s: %x", "Connection Destroyed", this)
}

void TCPConnection::handleReadEvent() {
    assert(current_channel->isInTiedThread());

    int read_n = in_buffer.readToBuffer(current_channel->getFd());
    LOG_WARN("%s %d %s", "total catch ", read_n, " bytes")

    if (read_n == 0) {
        if(++zero_num >= 10){
            handleCloseEvent();
            return ;
        }
    }else{
        zero_num = 0;
    }

    if (errno == EAGAIN) {
        errno = 0;
        return;
    }


    /// 作为 HTTP 服务器工作，检查是否读取完毕
    if (in_buffer.isCatchHTTPEnd()) {
        LOG_WARN("%s", "GET HTTP END")

        if (in_buffer.first() == 'G') {
            if (in_buffer.isCatchHTTPEnd()) {
                LOG_WARN("%s", "GET MESSAGE ENABLE WRITE")
                current_channel->enableWrite();
                return;
            }
        }

        if (in_buffer.first() == 'P') {

            // 确保 body 部分也被成功接受
            if (in_buffer.isCatchHTTPEnd()) {

                current_channel->putInLoop([this]() {

                    int n = in_buffer.readToBuffer(current_channel->getFd());

                    if (n == 0) {
                        current_channel->enableWrite();
                        LOG_WARN("%s", "POST MESSAGE ENABLE WRITE")
                    } else {
                        if (in_buffer.isCatchHTTPEnd()) {
                            current_channel->enableWrite();
                            LOG_WARN("%s", "POST MESSAGE ENABLE WRITE")
                        }
                    }
                });
                return;
            }
        }

    }


    /// 作为 TCP 服务器工作，报文尾部要求为“EOF”
    if (in_buffer.isCatchEOF()) {

        /// 如果防止单个占用过长时间，可以把下面的任务打包给主线程来做
        //FIXME :readCallBack(in_buffer.str(),in_buffer.size())

        current_channel->enableWrite();
        LOG_WARN("%s", "TCP MESSAGE ENABLE WRITE")

        return;
    }



}

void TCPConnection::handleWriteEvent() {
    assert(current_channel->isInTiedThread());

    /// 提前预留出空间
    std::string result;
    result.reserve(in_buffer.size());

    //std::cout<<in_buffer.str()<<std::endl;
    //std::cout<<in_buffer.size()<<std::endl;
    writeCallBack(in_buffer.str(), in_buffer.size(), result);

    size_t write_n = ::write(current_channel->getFd(), result.c_str(), result.size());

    in_buffer.clearHttpEnd();

    LOG_WARN("%s %d %s", "total write ", write_n, " bytes")

    //handleCloseEvent();
    current_channel->enableRead();
}

void TCPConnection::handleCloseEvent() {
    assert(current_channel->isInTiedThread());

    // 顶层应用用于析构 fd 相关的构造对象
    closeCallBack(current_channel->getFd());

    if (::close(current_channel->getFd()) == -1) {
        auto err = errno;
        LOG_ERROR("%s %d %s %d", "CLOSE FD", current_channel->getFd(), "ERRNO", err);
        if (err == EINTR) {
            current_channel->putInLoop(std::bind(&TCPConnection::handleCloseEvent, this));
        } else if (err == EBADF) {
            current_channel->destroy(); /// 通知 EventLoop 和 Poller 放弃控制权
            closeConnection();  /// 准备析构 Channel 和 Connection
            return;
        }
    }

    LOG_WARN("%d %s", current_channel, " is to closed");

    current_channel->destroy(); /// 通知 EventLoop 和 Poller 放弃控制权
    /// 放在了Connection的析构里
    closeConnection();  /// 准备析构 Channel 和 Connection
}


void TCPConnection::handleErrorEvent() {
//    auto error = current_channel->getConnectionError();
//    if (error == ConnectionError::TIMEOUT) {
//        if (in_buffer.str())
//    }
}