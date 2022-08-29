//
// Created by trc on 2021/10/19.
//

#ifndef TCPTEST_ACCEPTOR_H
#define TCPTEST_ACCEPTOR_H


#include <netinet/in.h>




//#ifdef __linux__
//#define POLLER_SIGNAL 0
//
//#include <sys/epoll.h>
//
//#else
#define POLLER_SIGNAL 1
//#endif

class TCPServer;
class Channel;

class Acceptor {
private:
    TCPServer *server;
    Channel *listen_channel;
    int listen_fd;
    int timeout;

public:
//#ifdef __linux__
//    explicit Acceptor(TCPServer *server, Channel *ch, int timeout_ms);
//#else
    explicit Acceptor(TCPServer *server, Channel *ch, int timeout_ms);
//#endif
//
    void createNewConnection();
};


#endif //TCPTEST_ACCEPTOR_H
