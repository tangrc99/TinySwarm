//
// Created by 唐仁初 on 2022/8/2.
//

#ifndef TINYSWARM_RPCCLIENT_H
#define TINYSWARM_RPCCLIENT_H


#include "IPAddress.h"
#include "Session.h"
#include "DynamicGenerator.h"
#include <memory>

#define MAX_SESSION_ON_CHANNEL 5

class RpcClient {
public:

    ///
    /// \param service
    /// \param proto_path
    explicit RpcClient(const std::string &service, const std::string &proto_path) : generator(
            std::make_shared<DynamicGenerator>(proto_path)) {

        service_ = generator->getServiceDescriptor(service);

        th_ = std::thread([this] { loop(); });
    }

    virtual ~RpcClient(){
        quit = true;

        if(th_.joinable())
            th_.join();
    }

    /// Create a new session on a new rpc channel.
    /// \param ipAddress The address of new session
    /// \param service ServiceDescriptor of session
    /// \return The ptr of new session
    std::shared_ptr<Session>
    createSessionOnNewChannel(const IPAddress &ipAddress) {

        auto channel = std::make_shared<RpcClientChannel>(ipAddress);
        channels.emplace(ipAddress.getAddress(), channel);

        return std::make_shared<Session>(channel, service_, generator, this);
    }

    /// Create a new session. If sessions of the old rpc channel is larger than MAX_SESSION_ON_CHANNEL,
    /// \param ipAddress The address of new session
    /// \param service ServiceDescriptor of session
    /// \return The ptr of new session
    std::shared_ptr<Session> createSession(const IPAddress &ipAddress) {

        auto pos = channels.find(ipAddress.getAddress());

        if (pos == channels.end())
            return createSessionOnNewChannel(ipAddress);

        auto &channel = pos->second;
        if (channel->sessions() > MAX_SESSION_ON_CHANNEL)
            return createSessionOnNewChannel(ipAddress);

        return std::make_shared<Session>(channel, service_, generator, this);
    }

    /// CallBack Function Used By Session.
    /// \return always true
    virtual bool SessionCallBack(){
        return true;
    }

private:

    void removeChannelLNU(int second) {
        auto now = time(nullptr);
        // map 遍历一次性只能够删除一个元素
        for (const auto &pair: channels) {
            if (pair.second->getLastTimestamp() - now > second && pair.second->sessions() == 0) {
                channels.erase(pair.first);
                return;
            }
        }
    }

    void loop() {
        while (!quit) {
            removeChannelLNU(30);
            sleep(5);
        }
    }


private:
    bool quit = false;

    std::thread th_;    // 用于处理client的周期任务

    using ipWithPort = std::string;

    std::unordered_map<ipWithPort, std::shared_ptr<RpcClientChannel>> channels;
    std::shared_ptr<DynamicGenerator> generator;

    const pf::ServiceDescriptor *service_;

};


#endif //TINYSWARM_RPCCLIENT_H
