//
// Created by 唐仁初 on 2022/9/11.
//

#ifndef TINYSWARM_PUBLISHER_H
#define TINYSWARM_PUBLISHER_H

#include <string>
#include <sw/redis++/redis++.h>
#include <iostream>

namespace manager::publisher {

    /// Class Publisher is the base class to process manager's messages between themselves. There is not one interface.
    class Publisher {

    public:

        /// Publish some message to other manager nodes.
        /// \param message Message to publish
        /// \return Is published
        virtual bool publishMessage(const std::string &message) {
            return false;
        }

    };


    class RedisPublisher : public Publisher {
    public:

        RedisPublisher() {
            redis = std::make_unique<sw::redis::Redis>("tcp://127.0.0.1:6379");
        }


        // 要求最后收到的 message 格式 "manager x.x.x.x port service alias"
        bool publishMessage(const std::string &message) override try {

            if (!redis)
                return false;

            redis->publish("manager", message);

            return true;

        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            return false;
        }


    private:
        // gossip_node :
        // redis prototype :
        std::unique_ptr<sw::redis::Redis> redis;

    };
}
#endif //TINYSWARM_PUBLISHER_H
