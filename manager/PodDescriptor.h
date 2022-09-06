//
// Created by 唐仁初 on 2022/8/2.
//

#ifndef TINYSWARM_PODDESCRIPTOR_H
#define TINYSWARM_PODDESCRIPTOR_H

#include <utility>
#include <nlohmann/json.hpp>
#include "WorkerRpcInterface.pb.h"

struct PodExitError {
    int exit;
    std::string reason;
    std::string out_file;
};


struct WorkerDescriptor;


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

    PodDescriptor(std::string service, std::string alias, ServiceType type, int alive, int port, WorkerDescriptor *wd,
                  std::vector<char *> exe_params,
                  std::vector<char *> docker_params, int restart);

    PodDescriptor() = default;

    [[nodiscard]] std::string toGossipMessage() const;

    [[nodiscard]] std::string toSnapshotMessage() const;

    nlohmann::json toJson();

};


#endif //TINYSWARM_PODDESCRIPTOR_H
