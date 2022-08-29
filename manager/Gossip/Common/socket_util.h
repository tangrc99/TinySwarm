//
// Created by 唐仁初 on 2021/10/20.
//

#ifndef TCPTEST_SOCKET_H
#define TCPTEST_SOCKET_H

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

static void setNonBlockAndCloseOnExec(int &sockfd)  /// 函数会更改 sockfd 的属性
{
    // non-block
    int flags = ::fcntl(sockfd, F_GETFL, 0);  ///得到文件状态标记
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockfd, F_SETFL, flags);    ///设置文件状态标记
    // FIXME check

    std::cout<<"set NonBlock\n";
    (void) ret;
}

class Socket {
private:
    int fd;

public:
    explicit Socket(int fd) : fd(fd) {
        setNonBlockAndCloseOnExec(fd);
    }

    explicit Socket(int domain, int type, int protocol) : fd(::socket(domain, type, protocol)) {
        setNonBlockAndCloseOnExec(fd);
    }

    int getFd() const { return fd; }

    ~Socket() {
        ::close(fd);
    }
};

#endif //TCPTEST_SOCKET_H
