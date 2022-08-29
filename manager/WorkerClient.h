//
// Created by 唐仁初 on 2022/8/2.
//

#ifndef TINYSWARM_WORKERCLIENT_H
#define TINYSWARM_WORKERCLIENT_H


#include "IPAddress.h"
#include "RpcSession.h"
#include <memory>

#define MAX_SESSION_ON_CHANNEL 5

class WorkerClient {
public:

    /// Create a new session on a new rpc channel.
    /// \param ipAddress The address of new session
    /// \return The ptr of new session
    std::shared_ptr<RpcSession> createSessionOnNewChannel(const IPAddress &ipAddress){

        auto channel = std::make_shared<RpcClientChannel>(ipAddress);
        channels.emplace(ipAddress.getAddress(),channel);

        return std::make_shared<RpcSession>(channel);
    }

    /// Create a new session. If sessions of the old rpc channel is larger than MAX_SESSION_ON_CHANNEL,
    /// the new session will be created on a new rpc channel
    /// \param ipAddress The address of new session
    /// \return The ptr of new session
    std::shared_ptr<RpcSession> createSession(const IPAddress &ipAddress){

        auto pos = channels.find(ipAddress.getAddress());
        if(pos == channels.end())
            return createSessionOnNewChannel(ipAddress);

        auto &channel = pos->second;
        if(channel->sessions()  > MAX_SESSION_ON_CHANNEL)
            return createSessionOnNewChannel(ipAddress);

        return std::make_shared<RpcSession>(channel);
    }


private:

    using ipWithPort = std::string;

    std::unordered_map<ipWithPort,std::shared_ptr<RpcClientChannel>> channels;

};


#endif //TINYSWARM_WORKERCLIENT_H
