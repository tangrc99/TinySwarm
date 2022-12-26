//
// Created by 唐仁初 on 2022/9/5.
//

#include "PodDescriptor.h"
#include "WorkerDescriptor.h"

namespace tiny_swarm::manager {


    PodDescriptor::PodDescriptor(std::string service, std::string alias, ServiceType type, int alive, int port,
                                 WorkerDescriptor *wd,
                                 std::vector<char *> exe_params, std::vector<char *> docker_params, int restart)
            : service_(std::move(service)), alias_(std::move(alias)), type_(type), alive_(alive), port_(port), wd_(wd),
              exe_params_(std::move(exe_params)), docker_params_(std::move(docker_params)), restart_(restart) {

        if (wd != nullptr)
            ip = wd->ip;
    }

    PodDescriptor::~PodDescriptor() {
        if (wd_ != nullptr)
            wd_->removePodDescriptor(this);
    };

    void PodDescriptor::bindWorker(WorkerDescriptor *wd) {
        wd_ = wd;
        if (wd != nullptr) {
            ip = wd->ip;
            wd->pods.emplace_back(this);
        }

    }

    std::string PodDescriptor::toGossipMessage() const {
        // example "127.0.0.1:8989 hello my_hello -1"
        return {wd_->ip + " " + std::to_string(wd_->port) + " " + service_ + " " + alias_ + std::to_string(port_)};
    }

    std::string PodDescriptor::toSnapshotMessage() const {
        std::string snapshot =
                service_ + " " + alias_ + " " + (type_ == host ? "host " : "docker ") + std::to_string(port_)
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

    nlohmann::json PodDescriptor::toJson() {
        nlohmann::json json;
        json["alias"] = alias_;
        json["connected"] = alive_;
        json["ip"] = ip;
        json["port"] = port_;
        json["restart"] = restart_;

        return json;
    }

}