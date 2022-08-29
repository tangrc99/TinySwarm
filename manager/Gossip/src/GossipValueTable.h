//
// Created by 唐仁初 on 2022/4/2.
//

#ifndef GOSSIP_GOSSIPVALUETABLE_H
#define GOSSIP_GOSSIPVALUETABLE_H


#include "Common/CommonStruct.h"
#include <string>
#include <map>
#include <mutex>



// 完成 Gossip 节点对用户的服务通信, 想直接使用 redis 的监听机制来实现
class GossipValueTable {
protected:

    std::mutex mtx;

    std::vector<MessagePack> messages_to_send;
    int sequence = 0;   //这里要求每台机器的时代序列是递增的，用于其他机器判断是否已经接受到该消息
    std::string name;   // 服务的名称

    time_t version = 0;

public:

    GossipValueTable() = default;

    virtual ~GossipValueTable() = default;

    virtual bool loadProperties() = 0;

    // Application Layer Realize the Messages to Send.
    bool getRecentMessages(std::vector<MessagePack> &messages){

        std::lock_guard<std::mutex> lg(mtx);

        time_t cur_time = time(nullptr);

        for(auto message = messages_to_send.begin(); message != messages_to_send.end();){
            if(message->expired_time <= cur_time){
                messages_to_send.erase(message);
                std::cout<< "message removed!\n";
            }else{
                message++;
            }
        }

        if(messages_to_send.empty())
            return false;

        messages = messages_to_send;


        return true;
    }

    virtual bool getAllMessages(std::vector<MessagePack> &messages) = 0;

    // Receive Message and Act from Other Nodes.
    virtual bool parseGossipMessages(const std::string & messages,time_t timestamp) = 0;

    // New Node Start Sync
    virtual bool syncFromOtherNode() = 0;

    // New Node requires Sync
    virtual bool requireSync() = 0;

    virtual void stopWatch() = 0;

    virtual void watchClientMessages() = 0;

    [[nodiscard]] std::string getName() const {
        return name;
    }

};



//仿照 Tensorrt 实现注册机制

class GossipValueTableDesk {
private:
    typedef std::map<std::string, GossipValueTable *> ServiceMap;
    ServiceMap *servMap;

    GossipValueTableDesk() : servMap(new ServiceMap){}

public:
    ~GossipValueTableDesk() {
        delete servMap;
    }

    static GossipValueTableDesk &getInstance() {
        static GossipValueTableDesk desk;
        return desk;
    }

    void registerService(GossipValueTable *service) {
        servMap->emplace(service->getName(), service);
    }

    GossipValueTable *getServicePtr(const std::string &serv_name) {
        auto serv = servMap->find(serv_name);
        return serv == servMap->end() ? nullptr : serv->second;
    }

    auto servMapBegin() {
        return servMap->begin();
    }

    auto servMapEnd() {
        return servMap->end();
    }
};

#define GossipTable_DESK RPCServiceDesk::getInstance()
#define REGISTERY_GossipTable(service) GossipTable_DESK.registerService(service);
#define GET_GossipTable_BY_NAME(name) GossipTable_DESK.getServicePtr(name);






#endif //GOSSIP_GOSSIPVALUETABLE_H
