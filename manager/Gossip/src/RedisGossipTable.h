//
// Created by 唐仁初 on 2022/4/4.
//

#ifndef GOSSIP_REDISGOSSIPTABLE_H
#define GOSSIP_REDISGOSSIPTABLE_H

#include "GossipValueTable.h"
#include "logUtils/Logging.h"
#include "Common/CommonParams.h"
#include <sw/redis++/redis++.h>
#include <sstream>
#include <iostream>

class RedisGossipTable final : public GossipValueTable {

private:
    std::unique_ptr<sw::redis::Redis> redis;
    sw::redis::Subscriber sub;

    std::list<std::string> worker_keys;

public:
    RedisGossipTable() : redis(std::make_unique<sw::redis::Redis>("tcp://127.0.0.1:6379")), sub(redis->subscriber()) {
        name = "redis";

        sub.subscribe("gossip");

        sub.on_message([this](const std::string &channel, const std::string &msg) {
            if (channel != "gossip") {
                LOG_ERROR("%s %s:%s", "NOT GOSSIP CHANNEL", channel.c_str(), msg.c_str());
                return;
            }

            LOG_WARN("%s %s", "subscribed msg", msg.c_str());

            if (msg == "exit")
                return;

            std::lock_guard<std::mutex> lg(mtx);

            time_t now = time(nullptr);

            auto is_valid = parseGossipMessages(msg, now);   // 信息流传播给本节点
            if (is_valid) {
                messages_to_send.emplace_back(msg, now, MESSAGES_ALIVE_TIME);   // 信息流传播给其他节点
            }
        });
    }

    bool loadProperties() override {

        return true;
    }

    // 要求最后收到的 message 格式 "manager x.x.x.x port service alias"
    bool parseGossipMessages(const std::string &message, time_t timestamp) override {

        LOG_DEBUG("%s %s", "message: ", message.c_str());

        std::string manager, ip, port, service, alias;
        std::stringstream ss(message);
        ss >> manager >> ip >> port >> service >> alias;
        std::string worker_key = ip + ":" + port;


        auto res = redis->hget(manager,"timestamp");

        if(!res.has_value() ||  res.value() > std::to_string(timestamp)){
            LOG_DEBUG("%s: %s", "old msg", message.c_str());
            return false;
        }

        auto tx = redis->transaction();

        tx.hset(manager, "timestamp", std::to_string(timestamp)).hset(manager, alias, worker_key).incr(
                worker_key);

        tx.exec();

        if(redis->get(ip + " " + port).value() == "1"){
            LOG_DEBUG("%s %s", "New Worker ",worker_key.c_str());
            worker_keys.emplace_back(worker_key);
        }


        return true;
//        std::stringstream ss(message);
//        std::string tmp;
//        while (std::getline(ss, tmp, '.')) {
//            int num = std::stoi(tmp);
//            if (num < 0 || num > 255) {
//                return false;
//            }
//        }
//
//        auto res = redis->zscore("Servers", message);
//
//        if (!res.has_value()) {
//            redis->zadd("Servers", message, static_cast<double>(timestamp));
//            LOG_DEBUG("%s %s", message.c_str(), "added");
//        } else {
//            if (res.value() < static_cast<double>(timestamp)) {
//                redis->zadd("Servers", message, static_cast<double>(timestamp));
//                LOG_DEBUG("%s %s", message.c_str(), "added");
//            } else {
//                LOG_DEBUG("%s: %s", "old msg", message.c_str());
//                return false;
//            }
//        }
//
//        return true;
    }

    bool getAllMessages(std::vector<MessagePack> &messages) override {

        std::vector<std::string> res;
        redis->zrange("Servers", 0, 100, std::back_inserter(res));

        if (res.empty())
            return false;

        for (const auto &str: res) {
            messages.emplace_back(str);
        }

        return true;
    }

    void stopWatch() override {
        redis->publish("gossip", "exit");
    }

    void watchClientMessages() override {
        sub.consume();
    }

    bool syncFromOtherNode() override { return true; }

    bool requireSync() override { return true; }
};


#endif //GOSSIP_REDISGOSSIPTABLE_H
