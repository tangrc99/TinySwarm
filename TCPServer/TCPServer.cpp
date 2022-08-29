//
// Created by trc on 2021/10/19.
//

#include "TCPServer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "TCPConnection.h"
#include "Poller.h"
#include "../logUtils/Logging.h"
#include <log4cxx/logmanager.h>


//#ifdef __linux__
//Poller *TCPServer::createPoll(const int &listen_fd) const {
//        return new Epoll(listen_fd);
//    }
//#else

Poller *TCPServer::createPoll(const int &listen_fd) {
    return new Poll(listen_fd);
}

//#endif

TCPServer::TCPServer(IPAddress addr, EventLoop *listen_loop, int thread_num, int timeout_ms) :
        local_address(addr),
        listen_loop(listen_loop) {

    writeCallBack = defaultWriteCallBack;
    readCallBack = defaultCallBack;
    errorCallBack = defaultCallBack;
    closeCallBack = defaultCloseCallBack;

    listen_fd = ::socket(local_address.famyly_t(), SOCK_STREAM, 0);
    LOG_DEBUG("%s: %d", "listen fd", listen_fd)

    if (listen_fd < 0) {
        LOG_FATAL("%s", "LISTEN FD INVALID")
    }

    poller = createPoll(listen_fd);

    listen_loop->setPoller(poller);

    thread_pool = new ThreadPool(thread_num, listen_loop);

    Channel *listen_channel = new Channel(listen_loop, listen_fd);

    LOG_DEBUG("%s: %x", "Listen Channel", listen_channel);

    acceptor = new Acceptor(this, listen_channel, timeout_ms);

    LOG_DEBUG("%s: %x", "New Acceptor",acceptor);

    listen_channel->setReadCallBack(std::bind(&Acceptor::createNewConnection, acceptor));
    poller->addListenChannel(listen_channel);

}

Channel *TCPServer::newConnection(const int &conn_fd) {
    auto new_channel = new Channel(thread_pool->getAnEventLoop(), conn_fd);

    LOG_WARN("%s %x %s %d","New ConnChannel",new_channel,"with fd",conn_fd)

    auto new_conn = std::make_shared<TCPConnection>(this, new_channel);

    new_conn->setReadCallBack(readCallBack);
    new_conn->setWriteCallBack(writeCallBack);
    new_conn->setErrorCallBack(errorCallBack);
    new_conn->setCloseCallBack(closeCallBack);

    connections.insert(new_conn);
    return new_channel;
}


void TCPServer::startListen() {

    LOG_DEBUG("%s","To bind fd");

    if (::bind(listen_fd, local_address.toSockAddrPtr(), local_address.addrlen()) < 0)
        exit(2);

    ::listen(listen_fd, 10);

    LOG_DEBUG("%s","Server start listen")

    listen_loop->loop();
}