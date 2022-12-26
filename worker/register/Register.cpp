//
// Created by 唐仁初 on 2022/9/3.
//

#include "Register.h"
#include <nlohmann/json.hpp>

namespace tiny_swarm::worker::registry {


    ETCDRegister::ETCDRegister(const std::string &url,int ttl) : client_(url),ttl_(ttl) {

        // 本函数将会用于 lease 用于的异常
        std::function<void(std::exception_ptr)> handler = [this](const std::exception_ptr &eptr) {
            try {
                if (eptr) {
                    std::rethrow_exception(eptr);
                }
            } catch (const std::runtime_error &e) {
                std::cerr << "Connection failure \"" << e.what() << "\"\n";
                expired_ = true;
            } catch (const std::out_of_range &e) {
                std::cerr << "Lease expiry \"" << e.what() << "\"\n";
                expired_ = true;
            }
        };

        auto lease = client_.leasegrant(ttl).get().value().lease();
        kp_ = std::make_shared<etcd::KeepAlive>(client_, handler, ttl, lease);
    }

    bool ETCDRegister::registry(const std::string &name, const std::string &info) {
        if (!path_in_etcd.empty()) {
            client_.rm_if(path_in_etcd, index_);
        }

        path_in_etcd = "/tinyswarm/workers/" + name;

        auto resp = client_.add(path_in_etcd, info, kp_->Lease()).get();

        if (resp.is_ok()) {
            index_ = resp.index();
            return true;
        } else {
            throw std::runtime_error(resp.error_message());
        }
        return false;
    }

    std::string ETCDRegister::generateRegistryInfo(const std::string &uri,const ParamMap &services,const ParamMap &status) {

        nlohmann::json info;

        info["uri"] = uri;
        info["services"] = services;
        info["status"] = status;

        return info.dump();
    }

    bool ETCDRegister::checkLeaseStatus() {
        return expired_ == false;
    }


}