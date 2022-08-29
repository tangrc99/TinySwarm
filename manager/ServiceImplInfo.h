//
// Created by 唐仁初 on 2022/8/2.
//

#ifndef TINYSWARM_SERVICEIMPLINFO_H
#define TINYSWARM_SERVICEIMPLINFO_H

#include <utility>

#include "WorkerRpcInterface.pb.h"
#include "WorkerDescriptor.h"

struct ServiceInfoError {
    int exit;
    std::string reason;
    std::string out_file;
};

struct ServiceImplInfo {

    std::string service_;
    std::string alias_;
    ServiceType type_;
    bool alive_;
    WorkerDescriptor *wd_ = nullptr;
    std::vector<char *> exe_params_;
    std::vector<char *> docker_params_;
    int restart_;

    ServiceInfoError *error = nullptr;

    ServiceImplInfo(string service, string alias, ServiceType type, int alive, WorkerDescriptor *wd,
                std::vector<char *> exe_params,
                std::vector<char *> docker_params, int restart)
            : service_(std::move(service)), alias_(std::move(alias)), type_(type), alive_(alive), wd_(wd),
              exe_params_(std::move(exe_params)), docker_params_(std::move(docker_params)), restart_(restart) {

    }

    ServiceImplInfo() = default;

    [[nodiscard]] std::string toGossipMessage() const {
        // example "127.0.0.1:8989 hello my_hello"
        return {wd_->ip + " " + std::to_string(wd_->port) + " " + service_ + " " + alias_};
    }

    [[nodiscard]] std::string toSnapshotMessage() const {
        std::string snapshot = service_ + " " + alias_ + " " + (type_ == host ? "host " : "docker ")
                               + std::to_string(alive_) + " " + wd_->ip + " " + std::to_string(wd_->port);

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


#endif //TINYSWARM_SERVICEIMPLINFO_H
