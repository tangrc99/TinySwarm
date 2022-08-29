//
// Created by trc on 2021/10/19.
//

#ifndef TCPTEST_POLLER_H
#define TCPTEST_POLLER_H

#include "Channel.h"
#include <iostream>
#include <vector>
#include <poll.h>
#include <map>
#include <netinet/in.h>
#include <cassert>
#include <thread>

#ifdef __linux__

#include <sys/epoll.h>

#endif

#define EPOLL_MAX 256


class Poller {
protected:
    typedef std::map<int, Channel *> ChannelList;    /// int 代表线程id
    ChannelList channels;

public:
    virtual void addChannel(Channel *new_channel) = 0;

    virtual void poll(std::vector<Channel *> &active_channel
    ) = 0;

    virtual void addListenChannel(Channel *listen_channel) = 0;

    virtual void removeChannel(Channel *ch) = 0;

    virtual void enableChannelWrite(Channel *ch) = 0;

    virtual void enableChannelRead(Channel *ch) = 0;

    virtual ~Poller() = default;
};


class Poll : public Poller {
private:
    std::vector<pollfd> pollfds;
public:

    explicit Poll(const int &listen_fd);

    explicit Poll() = default;

    auto fdFindChannel(int fd) ;

    void poll(std::vector<Channel *> &active_channel) override ;


    void addListenChannel(Channel *listen_channel) override ;

    void addChannel(Channel *new_channel) override;

    void removeChannel(Channel *ch) override ;

    void enableChannelWrite(Channel *ch) override ;

    void enableChannelRead(Channel *ch) override ;

    ~Poll() override = default;

};


#ifdef __linux__

class Epoll : public Poller {
private:
    int epoll_fd;
    std::vector<epoll_event> events;
    epoll_event active_events[EPOLL_MAX]{};


public:

    explicit Epoll(int listen_fd) : epoll_fd(::epoll_create(EPOLL_MAX + 1)) {
        epoll_event listen_event{EPOLLRDNORM,};
        listen_event.data.fd = listen_fd;
        events.emplace_back(listen_event);
        ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &listen_event);
    }

    Epoll() = default;

    auto fdFindChannel(int fd) {
        auto pos = channels.find(fd);
        return pos;
    }

    void poll(std::vector<Channel *> &active_channel) override {

        /// 如果epoll_wait没有发现新事件，将不会更新active_evnets
        int num = ::epoll_wait(epoll_fd, active_events, EPOLL_MAX, 50);

        for (auto &event:active_events) {
            if (event.events > 0) {

                assert(event.data.fd >= 0);

                std::cout << "Epoll: " << std::this_thread::get_id() << std::endl;
                std::cout << event.data.fd << ":" << event.events << std::endl;

                auto pos = fdFindChannel(event.data.fd);
                assert(pos != channels.end());

                pos->second->setRevent(event.events);
                active_channel.push_back(pos->second);

                event.events = 0;
                --num;

            }
            if (num == 0)
                break;
        }

    }

    /// 用于初始化
    void addListenChannel(Channel *listen_channel) override {
        channels.emplace(listen_channel->getFd(), listen_channel);
    }

    void addChannel(Channel *new_channel) override {
        epoll_event new_event{};
        new_event.data.fd = new_channel->getFd();
        new_event.events = new_channel->getEvent();
        channels.emplace(new_channel->getFd(), new_channel);
        events.emplace_back(new_event);
        /// 加入队列后再注册，防止临时变量析构
        int rt = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_event.data.fd, &events.back());
        if (rt == -1) {
            std::cout << "epoll_ctl rt: " << rt;
            std::cout << " ,errno:" << errno << std::endl;
        }
        std::cout << new_event.data.fd << "is added to epoll_events\n";
    }

    void removeChannel(Channel *ch) override {
        channels.erase(ch->getFd());
        std::cout << "Channel erased!\n";

        for (auto event = events.begin(); event != events.end(); event++) {
            if (event->data.fd == ch->getFd()) {
                /// 调用::close后不需要再主动去除epoll_event
                events.erase(event);
                std::cout << "event erased\n";
                break;
            }
        }

    }

    void modifyChannel(Channel *ch) override {
        auto pos = channels.find(ch->getFd());
        if (pos == channels.end()) {
            std::cout << "No such channel\n";
            std::abort();
        }
        pos->second->setEvent(EPOLLWRNORM | EPOLLHUP | EPOLLERR);

        for (auto &event : events) {
            if (event.data.fd == ch->getFd()) {

                event.events = (EPOLLWRNORM | EPOLLHUP | EPOLLERR);
                int rt = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, event.data.fd, &event);
                if (rt == -1) {
                    std::cout << "epoll_ctl rt: " << rt;
                    std::cout << " ,errno:" << errno << std::endl;
                }
                break;
            }
        }
        std::cout << "channel modified to write\n";
    }

    ~Epoll() override {
        ::close(epoll_fd);
    };

};

#endif


#endif //TCPTEST_POLLER_H
