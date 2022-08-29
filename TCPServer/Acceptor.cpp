//
// Created by trc on 2021/10/19.
//

#include <poll.h>
#include "Acceptor.h"
#include "TCPServer.h"
#include "Channel.h"
#include "../logUtils/Logging.h"


//#ifdef __linux__
//void Acceptor::createNewConnection() {
//    listen_channel->setRevent(0);
//
//    struct sockaddr_in6 addr{};
//    socklen_t len = sizeof addr;
//
//    int connfd = ::accept(listen_fd, (sockaddr *) &addr, &len);
//
//    if (connfd == -1) {
//        //LOG_FATAL("%s","connfd created failed");
//        return;
//    }
//
//
//
//    Channel *new_channel = server->newConnection(connfd);
//    if (new_channel == nullptr) {
//        //LOG_FATAL("%s", "Channel Created Failed");
//        exit(-1);
//    }
//    new_channel->setEvent((EPOLLRDNORM | EPOLLHUP | EPOLLERR)));
//    new_channel->updateChannelList();
//    std::cout << "NEW CONNECTION ESTABLISHED\n";
//
//    if (timeout > 0) {
//        new_channel->setAliveTime(timeout);
//    }
//
//}
//#else
void Acceptor::createNewConnection() {

    LOG_DEBUG("%s","try accept")

    listen_channel->setRevent(0);

    struct sockaddr_in addr{};
    socklen_t len = sizeof addr;

    int connfd = ::accept(listen_fd, (sockaddr *) &addr, &len);

    if (connfd == -1) {
        LOG_FATAL("%s", "connfd created failed")
        return;
    }

    LOG_DEBUG("%s", "CONNECTION ACCEPTED");

    Channel *new_channel = server->newConnection(connfd);

    if (new_channel == nullptr) {
        LOG_FATAL("%s", "Channel Created Failed")
        exit(-1);
    }
    new_channel->setEvent((POLLRDNORM | POLLHUP | POLLERR));
    new_channel->updateChannelList();

    LOG_DEBUG("%s", "NEW CONNECTION ESTABLISHED");

    if (timeout > 0) {
        LOG_DEBUG("%s","TIMER CREATED")
        new_channel->setAliveTime(timeout);
    }
}
//#endif


//#ifdef __linux__
//Acceptor::Acceptor(TCPServer *server, Channel *ch, int timeout_ms) :
//            server(server),
//            listen_fd(ch->getFd()),
//            listen_channel(ch),
//            timeout(timeout_ms) {
//        listen_channel->setEvent(EPOLLRDNORM);
//        listen_channel->setRevent(0);
//    }
//#else
Acceptor::Acceptor(TCPServer *server, Channel *ch, int timeout_ms) :
        server(server),
        listen_fd(ch->getFd()),
        listen_channel(ch),
        timeout(timeout_ms) {
    listen_channel->setEvent(POLLRDNORM);
    listen_channel->setRevent(0);
}

//#endif
