//
// Created by 唐仁初 on 2022/8/2.
//

#ifndef TINYSWARM_WORKERDESCRIPTOR_H
#define TINYSWARM_WORKERDESCRIPTOR_H

#include "Session.h"
#include "PodDescriptor.h"

namespace manager {


    /// Struct WorkerDescriptor describes a worker status view. It contains rpc connection to worker, created pods on worker
    /// and other useful information.
    struct WorkerDescriptor {
        std::string ip;
        int port;

        bool alive;

        std::shared_ptr<Session> session;   // 用于通信的连接

        std::list<std::string> services;    // 节点上提供的服务
        std::list<PodDescriptor *> pods; // 当前节点上运行的pod


        /// Interface to simplify a remove operation. This function is called after a pod is transferred or deleted on
        /// this worker node.
        /// \param pd PodDescriptor to be removed.
        void removePodDescriptor(PodDescriptor *pd) {
            for (auto it = pods.begin(); it != pods.end();) {
                if (*it == pd) {
                    pods.erase(it);
                    return;
                } else {
                    it++;
                }
            }
        }

        /// Check is port current available on this worker node. This function will be called when manager select this
        /// worker to create pod.
        /// \param p Port
        /// \return Is port available on this worker
        bool portAvailable(int p) {
            for (auto &pod: pods) {
                if (pod->port_ == p)
                    return false;
            }
            return true;
        }

        /// Generate message with json format
        /// \return Generated message
        nlohmann::json toJson() {
            nlohmann::json json;

            json["ip"] = ip;
            json["port"] = port;
            json["status"] = (alive ? "connected" : "down");
            json["service"] = services;
            std::vector<std::string> pod_names;
            pod_names.reserve(pods.size());
            for (auto pod: pods) {
                pod_names.emplace_back(pod->alias_);
            }
            json["pods"] = pod_names;

            return json;
        }

    };
}
#endif //TINYSWARM_WORKERDESCRIPTOR_H
