//
// Created by trc on 2021/10/19.
//

#include "Poller.h"
#include "../logUtils/Logging.h"

Poll::Poll(const int &listen_fd) {
    pollfd pfd{};
    pfd.fd = listen_fd;
    pfd.events = POLLRDNORM;
    pfd.revents = 0;
    pollfds.push_back(pfd);

}

auto Poll::fdFindChannel(int fd) {
    auto pos = channels.find(fd);
    assert(pos != channels.end());
    return pos;
}

void Poll::poll(std::vector<Channel *> &active_channel) {
    //assert(!pollfds.empty());

    int num = ::poll(&*pollfds.begin(), pollfds.size(), 5);

    for (auto pollfd: pollfds) {
        assert(pollfd.fd >= 0);

        if (pollfd.revents > 0) {

            LOG_DEBUG("%s:%d,%d","fd with event",pollfd.fd,pollfd.revents)

            auto pos = fdFindChannel(pollfd.fd);
            pos->second->setRevent(pollfd.revents);
            active_channel.push_back(pos->second);
            --num;
            pollfd.revents = 0;
        }
        if (num == 0)
            break;
    }
}

void Poll::addListenChannel(Channel *listen_channel) {
    channels.emplace(listen_channel->getFd(), listen_channel);
}

void Poll::addChannel(Channel *new_channel)  {
    pollfd new_pollfd{};
    new_pollfd.fd = new_channel->getFd();
    new_pollfd.events = new_channel->getEvent();
    channels.emplace(new_channel->getFd(), new_channel);
    pollfds.emplace_back(new_pollfd);

    LOG_DEBUG("%d %s",new_pollfd.fd, "is added to pollfds")
}

void Poll::removeChannel(Channel *ch) {
    channels.erase(ch->getFd());

    for (auto pollfd = pollfds.begin(); pollfd != pollfds.end(); pollfd++) {
        if (pollfd->fd == ch->getFd()) {
            pollfds.erase(pollfd);
            break;
        }
    }

}

void Poll::enableChannelWrite(Channel *ch) {
    auto pos = channels.find(ch->getFd());
    if (pos == channels.end()) {
        LOG_FATAL("%s %x","No channel",ch)
        return ;
    }
    pos->second->setEvent(POLLWRNORM | POLLHUP | POLLERR | POLLRDNORM);

    for (auto &pollfd : pollfds) {
        if (pollfd.fd == ch->getFd()) {
            pollfd.events = (POLLWRNORM | POLLHUP | POLLERR |POLLRDNORM);
            break;
        }
    }
}

void Poll::enableChannelRead(Channel *ch) {
    auto pos = channels.find(ch->getFd());
    if (pos == channels.end()) {
        LOG_FATAL("%s %x","No channel",ch)
        return ;
    }
    pos->second->setEvent(POLLRDNORM | POLLHUP | POLLERR);

    for (auto &pollfd : pollfds) {
        if (pollfd.fd == ch->getFd()) {
            pollfd.events = (POLLRDNORM | POLLHUP | POLLERR);
            break;
        }
    }
}
