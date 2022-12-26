//
// Created by 唐仁初 on 2022/8/2.
//

#ifndef TINYSWARM_PODDESCRIPTOR_H
#define TINYSWARM_PODDESCRIPTOR_H

#include <utility>
#include <nlohmann/json.hpp>
#include "manager/protos/WorkerRpcInterface.pb.h"

namespace tiny_swarm::manager {

    /// Struct PodExitError contains all needed pod exit information.
    struct PodExitError {
        int exit;
        std::string reason;
        std::string out_file;
    };


    struct WorkerDescriptor;


    /// Struct PodDescriptor describes to pod view in manager node. It contains all information needed.
    struct PodDescriptor {

        std::string service_;   // 服务名称
        std::string alias_;     // 服务的唯一标识
        ServiceType type_;      // 服务的运行类型
        bool alive_;
        WorkerDescriptor *wd_ = nullptr;
        std::vector<char *> exe_params_;
        std::vector<char *> docker_params_;
        std::string ip; // 被调度到的地址
        int port_;  // 将要工作的端口号
        int restart_;

        PodExitError *error = nullptr;

        /// Construction RAII.
        PodDescriptor(std::string service, std::string alias, ServiceType type, int alive, int port,
                      WorkerDescriptor *wd,
                      std::vector<char *> exe_params,
                      std::vector<char *> docker_params, int restart);

        /// Construct an empty descriptor. Used to construct and then assignment.
        PodDescriptor() = default;

        ~PodDescriptor();

        /// Bind this pod descriptor to a worker descriptor.
        /// \param wd Worker descriptor to bind
        void bindWorker(WorkerDescriptor *wd);

        /// Generate gossip message.
        /// \return generated message
        [[nodiscard]] std::string toGossipMessage() const;

        /// Generate snapshot message.
        /// \return generated message
        [[nodiscard]] std::string toSnapshotMessage() const;

        /// Generate json type message.
        /// \return generated message
        nlohmann::json toJson();

    };

}
#endif //TINYSWARM_PODDESCRIPTOR_H
