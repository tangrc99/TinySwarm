//
// Created by 唐仁初 on 2022/8/2.
//

#ifndef TINYSWARM_PODDESCRIPTOR_H
#define TINYSWARM_PODDESCRIPTOR_H

#include <utility>

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
    int port_;  // 将要工作的端口号
    int restart_;

    PodExitError *error = nullptr;

    PodDescriptor(string service, string alias, ServiceType type, int alive, int port, WorkerDescriptor *wd,
                  std::vector<char *> exe_params,
                  std::vector<char *> docker_params, int restart)
            : service_(std::move(service)), alias_(std::move(alias)), type_(type), alive_(alive), port_(port), wd_(wd),
              exe_params_(std::move(exe_params)), docker_params_(std::move(docker_params)), restart_(restart) {

    }

    PodDescriptor() = default;

    [[nodiscard]] std::string toGossipMessage() const {
        // example "127.0.0.1:8989 hello my_hello"
        return {wd_->ip + " " + std::to_string(wd_->port) + " " + service_ + " " + alias_ + std::to_string(port_)};
    }

    [[nodiscard]] std::string toSnapshotMessage() const {
        std::string snapshot = service_ + " " + alias_ + " " + (type_ == host ? "host " : "docker ") + std::to_string(port_)
                + " " + std::to_string(alive_) + " " + wd_->ip + " " + std::to_string(wd_->port);

        snapshot += " {";
        for (auto &params: exe_params_) {
            snapshot += params;
            snapshot += ",";
        }
        snapshot += "} {";

        for (auto &params: exe_params_) {
            snapshot += params;
            snapshot += ",";
        }
        snapshot += "} ";

        snapshot += std::to_string(restart_);
        return snapshot;
    }


};


#endif //TINYSWARM_PODDESCRIPTOR_H
