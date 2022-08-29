//
// Created by 唐仁初 on 2022/4/2.
//

#ifndef GOSSIP_GOSSIPNODE_H
#define GOSSIP_GOSSIPNODE_H

#include "GossipValueTable.h"
#include "RedisGossipTable.h"
#include "Common/CommonStruct.h"
#include "Common/CommonParams.h"
#include "IOHandler.h"
#include "Common/IPAddress.h"
#include "logUtils/Logging.h"
#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <csignal>


// 实现 Gossip 节点内部的通信
class GossipNode {
private:

    std::vector<GossipPtr> active_nodes;
    std::vector<std::shared_ptr<IPAddress>> sleep_nodes;
    std::unique_ptr<GossipValueTable> value_table;
    bool quit = false;

    std::unique_ptr<IOHandler> ioHandler;

    int listen_fd{};

    std::unique_ptr<CommandQueue> command_queue;

    std::thread watch_thread;

    void createListenPort(const IPAddress &address) {
        listen_fd = ::socket(address.famyly_t(), SOCK_STREAM, 0);
        LOG_DEBUG("%s: %d", "listen fd", listen_fd)

        if (listen_fd < 0) {
            std::cerr << "Can not create listen fd\n";
            exit(-1);
        }

        if (::bind(listen_fd, address.toSockAddrPtr(), address.addrlen()) < 0) {
            std::cerr << "Bind Listen fd Error\n";
            exit(2);
        }

        ::listen(listen_fd, 10);

        active_nodes.emplace_back(
                std::make_shared<GossipChannel>(listen_fd, std::make_shared<IPAddress>(address), false));
    }

    static void signal_handle(int ){
        LOG_WARN("%s","SIGNAL UP");
    }

public:
    explicit GossipNode(IPAddress local_address, const std::string &file_name, int nums = 3) {
        value_table = std::make_unique<RedisGossipTable>();
        // create the listen port
        createListenPort(local_address);

        ioHandler = std::make_unique<IOHandler>(active_nodes, nums);
        ioHandler->setParseFunction(std::bind(&GossipNode::parseMessages,this,std::placeholders::_1,std::placeholders::_2));
        ioHandler->setPullFunction([this](auto && PH1) { pullData(std::forward<decltype(PH1)>(PH1)); });

        loadProperties(file_name);

        // timed command in the event loop
        command_queue = std::make_unique<CommandQueue>();
        command_queue->addRepeatedCommand([this] { sendRecentMessage(); }, MESSAGE_SPREAD_TIME);
        command_queue->addRepeatedCommand([this] { sendAllMessage(); }, ANTI_ENTROPY_TIME);
        command_queue->addRepeatedCommand([this] { removeNotAliveNodes(); }, NODE_ALIVE_TIME);

    }

    // send received message to value table
    bool parseMessages(const std::string &message, time_t timestamp) {
        return value_table->parseGossipMessages(message, timestamp);
    }


    void loadProperties(const std::string &file_name) {
        std::ifstream in(file_name);

        std::string line;
        while (std::getline(in, line)) {

            if(line.empty())
                continue;

            if(line[0] == '#' || line[0] == '/')
                continue;

            auto peer_addr = std::make_shared<IPAddress>(line, AF_INET);

            int fd = ::socket(AF_INET, SOCK_STREAM, 0);
            auto res = ::connect(fd, peer_addr->toSockAddrPtr(), peer_addr->addrlen());
            if (res == 0) {
                ioHandler->addAliveNode(std::make_shared<GossipChannel>(fd, peer_addr));
                LOG_DEBUG("%s %s", "connected", peer_addr->getAddress().c_str());
                //std::cout << "connected " << peer_addr->getAddress() << std::endl;
            } else {
                sleep_nodes.emplace_back(peer_addr);
                LOG_DEBUG("%s %s", "not connected", peer_addr->getAddress().c_str());
                //std::cout << "not connected " << peer_addr->getAddress() << std::endl;
            }
        }
    }

    bool sendRecentMessage() {
        LOG_DEBUG("%s", "sendRecentMessage Called");
        std::vector<MessagePack> messages;
        auto res = value_table->getRecentMessages(messages);

        if (!res)
            return false;
        LOG_DEBUG("%s %d", "to send msg,nums", messages.size());

        //std::cout << "ready to send messages!\n";
        //完成定时任务,发送信息/不发送信息
        ioHandler->randomSendMessages(messages);

        return true;
    }

    bool sendAllMessage() {
        std::vector<MessagePack> messages;
        auto res = value_table->getAllMessages(messages);

        if (!res)
            return false;
        LOG_DEBUG("%s %d", "to send all data,size", messages.size());

        //完成定时任务,发送信息/不发送信息
        ioHandler->randomSendMessages(messages);

        return true;
    }

    void pullData(const GossipPtr& node){

        // 拉取全部数据比较耗费时间，所以不能放在事件处理中，可能阻塞其他时间，而是处理完当前事件后处理
        command_queue->addCommand([this,&node](){
            std::vector<MessagePack> messages;
            auto res = value_table->getAllMessages(messages);
            if (!res){
                // FIXME : 这里需要回复对方自身也是空的
            }
            ioHandler->sendMessages(messages,node);
        },USR_COMMAND_DELAY);
    }

    void EventLoop() {

        watch_thread = std::thread([this]() {
            while (!quit) {
                watchValueTable();
            }
            std::cout << "watch exit\n";
        });

        //signal(SIGALRM,signal_handle);

        while (!quit) {
            time_t wait_time = std::max(command_queue->getUrgentTime() - time(nullptr), long(0));

            //这里需要有一个带时间限制的消息队列
            ioHandler->waitFor(wait_time * 1000);
            ioHandler->handleReceivedMessages();

            //完成定时任务,发送信息/不发送信息
            command_queue->runExpiredCommand();
        }

        std::cout << "Event Loop Exit\n";

        value_table->stopWatch();

        if (watch_thread.joinable()) {
            std::cout << "watch join\n";
            watch_thread.join();
        }

    }

    void watchValueTable() {
        value_table->watchClientMessages();
    }

    void removeNotAliveNodes() {
        auto cur_time = time(nullptr);

        // 跳过对监听频道的检查
        for (auto node = active_nodes.begin() + 1; node != active_nodes.end();) {
            if ((*node)->recent_time - cur_time >= NODE_ALIVE_TIME) {
                node = active_nodes.erase(node);
            } else {
                node++;
            }
        }
    }

    // 定期连接不活跃的节点，尝试修复节点网络
    void connectNotAliveNodes() {
        for (auto peer_addr = sleep_nodes.begin(); peer_addr != sleep_nodes.end();) {
            int fd = ::socket(AF_INET, SOCK_STREAM, 0);
            auto res = ::connect(fd, (*peer_addr)->toSockAddrPtr(), (*peer_addr)->addrlen());
            if (res == 0) {
                //active_nodes.emplace_back(std::make_shared<GossipChannel>(fd,peer_addr));
                ioHandler->addAliveNode(std::make_shared<GossipChannel>(fd, *peer_addr));
                std::cout << "connected " << (*peer_addr)->getAddress() << std::endl;
                sleep_nodes.erase(peer_addr);
            } else {
                peer_addr++;
            }
        }
    }


    /// 下面是留作用户接口调用的函数入口



    void exitEventLoop() {
        quit = true;
    }

    void showActiveNodes() {
        command_queue->addCommand([this]() {
            LOG_DEBUG("%s","showActiveNodes Called");
            for (auto &node: active_nodes) {
                std::cout << node->isPeer << "  " << node->addr->getAddress() << "\n";
            }
        }, USR_COMMAND_DELAY);
    }

    void showSleepNodes() {
        command_queue->addCommand([this]() {
            LOG_DEBUG("%s","showSleepNodes Called");
            for (auto &node: sleep_nodes) {
                std::cout << node->getAddress() << " ";
            }
            std::cout << std::endl;
        }, USR_COMMAND_DELAY);
    }


    // 显示当前使用的 value table 类型地址信息等
    void showValueTableInfo() {

    }

    // 主要要求同步数据
    void pushData(const std::vector<std::string> &addr){
        if(active_nodes.size() <= 1){
            std::cout<< "Not Connected to Other Node\n";
            return ;
        }


        std::vector<MessagePack> msg{MessagePack("push")};

        if(addr.empty()){
            ioHandler->sendMessages(msg,active_nodes[1]);
        }

        // FIXME: 进行类型检查

        for(auto & ip : addr){
            IPAddress ipAddress(ip,AF_INET);


        }


    }


};


#endif //GOSSIP_GOSSIPNODE_H
