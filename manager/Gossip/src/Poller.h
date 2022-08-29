//
// Created by trc on 2021/10/19.
//

#ifndef TCPTEST_POLLER_H
#define TCPTEST_POLLER_H

#include "Common/CommonStruct.h"
#include <iostream>
#include <vector>
#include <poll.h>
#include <map>
#include <netinet/in.h>
#include <cassert>
#include <thread>



#define EPOLL_MAX 256


class Poller {
protected:
    typedef std::map<int, GossipPtr> ChannelList;    /// int 代表线程id
    ChannelList channels;

    std::vector<pollfd> pollfds;

public:

    explicit Poller(GossipPtr listen_channel){
        channels.emplace(listen_channel->fd,listen_channel);
        pollfd p_fd{listen_channel->fd,static_cast<short>(listen_channel->events),static_cast<short>(listen_channel->revents)};
        pollfds.emplace_back(p_fd);
    }


    virtual void addGossipServer(GossipPtr new_channel) {
        channels.emplace(new_channel->fd,new_channel);
        pollfd p_fd{new_channel->fd,static_cast<short>(new_channel->events),static_cast<short>(new_channel->revents)};
        pollfds.emplace_back(p_fd);
    }

    auto fdFindChannel(int fd) {
        auto pos = channels.find(fd);
        assert(pos != channels.end());
        return pos;
    }

    virtual void poll(long ms){

        int num = ::poll(&*pollfds.begin(),pollfds.size(),ms);

        if(num <= 0){
//            if(num < 0)
//                std::cerr<<errno;
            return ;
        }


        for(auto &pollfd : pollfds){

            assert(pollfd.fd >= 0);

            if(pollfd.revents > 0){
                //std::cout<<pollfd.fd<<" "<<pollfd.revents<<"\n";

                auto pos = fdFindChannel(pollfd.fd);
                pos->second->revents = pollfd.revents;

                --num;
                pollfd.revents = 0;
            }
            if (num == 0)
                break;
        }

    }

    virtual void addListenChannel(GossipPtr listen_channel) { }

    virtual void removeChannel(GossipPtr &ch) {
        auto pos = channels.find(ch->fd);
        channels.erase(pos);

        for(auto pollfd = pollfds.begin(); pollfd!=pollfds.end();pollfd++){
            if(pollfd->fd == ch ->fd){
                pollfds.erase(pollfd);
                break;
            }
        }
    }

    virtual void enableChannelWrite(GossipPtr ch) {}

    virtual void enableChannelRead(GossipPtr ch) {}

    virtual ~Poller() = default;
};





#endif //TCPTEST_POLLER_H
