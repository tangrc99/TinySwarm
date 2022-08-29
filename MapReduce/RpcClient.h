//
// Created by 唐仁初 on 2022/8/2.
//

#ifndef TINYSWARM_WORKERCLIENT_H
#define TINYSWARM_RPCCLIENT_H


#include "IPAddress.h"
#include "Session.h"
#include "DynamicGenerator.h"
#include <memory>

#define MAX_SESSION_ON_CHANNEL 5

namespace mapreduce {

    class RpcClient {
    public:

        explicit RpcClient(const std::string &service, const std::string &proto_path) : generator(std::make_shared<DynamicGenerator>(proto_path)) {

            service_ = generator->getServiceDescriptor(service);
        }

        /// Create a new session on a new rpc channel.
        /// \param ipAddress The address of new session
        /// \param service ServiceDescriptor of session
        /// \return The ptr of new session
        std::shared_ptr<Session>
        createSessionOnNewChannel(const IPAddress &ipAddress) {

            auto channel = std::make_shared<RpcClientChannel>(ipAddress);
            channels.emplace(ipAddress.getAddress(), channel);

            return std::make_shared<Session>(channel, service_, generator);
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

            return std::make_shared<Session>(channel, service_, generator);
        }

        void removeChannelLNU(int second) {
            auto now = time(nullptr);
            // map 遍历一次性只能够删除一个元素
            for (const auto &pair: channels) {
                if (pair.second->getLastTimestamp() - now > second) {
                    channels.erase(pair.first);
                    return;
                }
            }
        }

    private:

        using ipWithPort = std::string;

        std::unordered_map<ipWithPort, std::shared_ptr<RpcClientChannel>> channels;
        std::shared_ptr<DynamicGenerator> generator;

        const pf::ServiceDescriptor *service_;

    };
}


#endif //TINYSWARM_WORKERCLIENT_H
