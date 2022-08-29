//
// Created by 唐仁初 on 2022/4/2.
//

#ifndef GOSSIP_IOHANDLER_H
#define GOSSIP_IOHANDLER_H

#include "Common/CommonStruct.h"
#include "Poller.h"
#include "Common/HTTPData.h"
#include "logUtils/Logging.h"
#include <utility>
#include <vector>
#include <random>
#include <unistd.h>
#include <Common/HTTPParser.h>
#include <sys/fcntl.h>

class IOHandler {
private:

    std::unique_ptr<Poller> poller; //用于监听读事件
    std::vector<GossipPtr> &alive_nodes;

    int epidemic_nums;

    std::function<bool(std::string &, time_t)> parseMessage;
    std::function<void(const GossipPtr &node)> pull;

    static void setNonBlockAndCloseOnExec(int &sockfd)  /// 函数会更改 sockfd 的属性
    {
        int flags = ::fcntl(sockfd, F_GETFL, 0);  ///得到文件状态标记
        flags |= O_NONBLOCK;
        int ret = ::fcntl(sockfd, F_SETFL, flags);    ///设置文件状态标记
        (void) ret;
    }


public:


    explicit IOHandler(std::vector<GossipPtr> &nodes, int nums = 3) : alive_nodes(nodes), epidemic_nums(nums) {
        if (nums <= 0) {
            std::cerr << "ERROR EPIDEMIC NUMS <= 0\n";
            exit(-1);
        } else if (nodes.empty()) {
            std::cerr << "ERROR INITIAL NODES <= 0\n";
            exit(-1);
        }

        alive_nodes[0]->events = POLLRDNORM | POLLHUP | POLLERR;
        poller = std::make_unique<Poller>(alive_nodes[0]);
    }

    void setParseFunction(std::function<bool(const std::string &, time_t)> functor) {
        parseMessage = functor;
    }

    void setPullFunction(std::function<void(const GossipPtr &)> functor) {
        pull = functor;
    }

    void randomSendMessages(const std::vector<MessagePack> &messages) {

        if (alive_nodes.size() == 1 && !alive_nodes[0]->isPeer)
            return;

        // 随机算法选择几个 alive_nodes 来发送字符串
        std::random_device rd;
        std::default_random_engine eng(rd());

        std::uniform_int_distribution<int> distr(1, static_cast<int>(alive_nodes.size() - 1));

        std::vector<int> rand_res;

        while (rand_res.size() < epidemic_nums) {
            int rand = distr(eng);

            if (std::find(rand_res.begin(), rand_res.end(), rand) == rand_res.end())
                rand_res.emplace_back(rand);

            if (rand_res.size() >= alive_nodes.size() - 1)
                break;
        }

        std::string message_to_send;

        for (const auto &message: messages) {

            HTTPParser::getReqMessage(message_to_send, {"POST", "", "1.1"},
                                      {{"timestamp", std::to_string(message.born_time)}},
                                      message.message);

            // 向随机得到的每个结点发送数据
            auto total = message_to_send.size();

            if (!rand_res.empty()) {
                for (auto &rand: rand_res) {

                    LOG_DEBUG("%s %s", "send to ", alive_nodes[rand]->addr->getAddress().c_str());

                    size_t pos = 0;

                    while (pos < total) {
                        // FIXME :这里可能会抛出异常，还没有考虑
                        pos += ::write(alive_nodes[rand]->fd, message_to_send.substr(pos).c_str(), total - pos);
                    }
                }
            }
        }
    }

    void spreadMessage(const MessagePack &message, const GossipPtr &source) {

        if (alive_nodes.size() <= 2 && !alive_nodes[0]->isPeer)
            return;

        // 随机算法选择几个 alive_nodes 来发送字符串
        std::random_device rd;
        std::default_random_engine eng(rd());

        std::uniform_int_distribution<int> distr(1, static_cast<int>(alive_nodes.size() - 1));

        std::vector<int> rand_res;

        while (rand_res.size() < epidemic_nums) {
            int rand = distr(eng);

            if (std::find(rand_res.begin(), rand_res.end(), rand) == rand_res.end() && alive_nodes[rand] != source)
                rand_res.emplace_back(rand);

            if (rand_res.size() >= alive_nodes.size() - 2)  // 去除自身和消息源节点
                break;
        }

        std::string message_to_send;

        HTTPParser::getReqMessage(message_to_send, {"POST", "", "1.1"},
                                  {{"timestamp", std::to_string(message.born_time)}},
                                  message.message);

        // 向随机得到的每个结点发送数据
        auto total = message_to_send.size();

        if (!rand_res.empty()) {
            for (auto &rand: rand_res) {

                LOG_DEBUG("%s %s", "send to ", alive_nodes[rand]->addr->getAddress().c_str());

                size_t pos = 0;

                while (pos < total) {
                    // FIXME :这里可能会抛出异常，还没有考虑
                    pos += ::write(alive_nodes[rand]->fd, message_to_send.substr(pos).c_str(), total - pos);
                }
            }
        }
    }

    // 发送消息给指定的节点
    void sendMessages(const std::vector<MessagePack> &messages, const GossipPtr &node) {

        std::string message_to_send;
        for (const auto &message: messages) {

            HTTPParser::getReqMessage(message_to_send, {"POST", "", "1.1"},
                                      {{"timestamp", std::to_string(message.born_time)}},
                                      message.message);

            // 向随机得到的每个结点发送数据
            auto total = message_to_send.size();

            LOG_DEBUG("%s %s", "send to ", node->addr->getAddress().c_str());

            size_t pos = 0;

            while (pos < total) {
                // FIXME :这里可能会抛出异常，还没有考虑
                pos += ::write(node->fd, message_to_send.substr(pos).c_str(), total - pos);
            }
        }

    }

//    void sendMessageByAddress(const MessagePack &message,const IPAddress &address){
//        std::string message_to_send;
//
//        HTTPParser::getReqMessage(message_to_send, {"POST", "", "1.1"},
//                                  {{"timestamp", std::to_string(message.born_time)}},
//                                  message.message);
//
//        // 向随机得到的每个结点发送数据
//        auto total = message_to_send.size();
//
//        LOG_DEBUG("%s %s", "send to ", address.getAddress().c_str());
//
//        size_t pos = 0;
//
//        while (pos < total) {
//            // FIXME :这里可能会抛出异常，还没有考虑
//            pos += ::write(node->fd, message_to_send.substr(pos).c_str(), total - pos);
//        }
//    }
//
//    void sendMessagesByAddress(const std::vector<MessagePack> &messages,const IPAddress &address){
//
//    }

    void addAliveNode(const GossipPtr &node) {
        node->events = POLLRDNORM | POLLHUP | POLLERR;
        poller->addGossipServer(node);
        alive_nodes.emplace_back(node);
    }

    void waitFor(long ms) {
        poller->poll(ms);
    }

// 用来处理接到的消息
    void handleReceivedMessages() {
        for (auto &node: alive_nodes) {
            if (node->revents > 0) {

                node->recent_time = time(nullptr);

                if (!node->isPeer) {
                    node->revents = 0;
                    LOG_DEBUG("%d %s", node->fd, "Registry Event");
                    handleRegistryMessage(node);
                    return;
                }

                LOG_DEBUG("%d %s", node->fd, "Gossip Event");
                handleGossipMessage(node);
            }
        }
    }

    void handleRegistryMessage(GossipPtr &node) {

        struct sockaddr_in addr{};
        socklen_t len = sizeof addr;

        int fd = ::accept(node->fd, (sockaddr *) &addr, &len);

        setNonBlockAndCloseOnExec(fd);

        // FIXME : 端口号存在字节序的问题

        IPAddress peer_address(addr);

        LOG_WARN("%s %s", "new node", peer_address.getAddress().c_str());
        //std::cout << peer_address.getAddress() << "\n";

        GossipPtr new_channel = std::make_shared<GossipChannel>(fd, std::make_shared<IPAddress>(addr));

        addAliveNode(new_channel);
        //这里需要同步数据
    }


    void handleGossipMessage(GossipPtr &node) {

        if (node->revents & POLLHUP) {

            poller->removeChannel(node);

            auto pos = std::find(alive_nodes.begin(), alive_nodes.end(), node);
            if (pos != alive_nodes.end())
                alive_nodes.erase(pos);

            LOG_WARN("%s %s", "node down", node->addr->getAddress().c_str());

            return;
        }

        auto size = node->readToBuffer();

        LOG_DEBUG("%s %d","total read: ",size);

        if (node->buffer.isCatchHTTPEnd()) {

            bool finished = false;

            HTTPData HTTPdata(node->buffer.str(), HTTP_TYPE::REQUEST);

            std::string message = HTTPdata.getHTTPBody();
            time_t timestamp = std::stol(HTTPdata.getParamValue("timestamp"));

            LOG_WARN("%s %s", "received", message.c_str());
            LOG_WARN("%s %d", "timestamp", timestamp);

            finished = node->buffer.moveNextHttpPack();

            if (message == "push") {
                pull(node);
            } else {

                bool not_received = parseMessage(message, timestamp);
                if(not_received){
                    MessagePack msg_pack(message,timestamp,1);
                    spreadMessage(msg_pack, node);
                }
            }

            if (finished)
                node->revents = 0;
        }
    }


};


#endif //GOSSIP_IOHANDLER_H
