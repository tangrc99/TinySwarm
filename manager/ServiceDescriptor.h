//
// Created by 唐仁初 on 2022/9/2.
//

#ifndef TINYSWARM_SERVICEDESCRIPTOR_H
#define TINYSWARM_SERVICEDESCRIPTOR_H

#include "PodDescriptor.h"
#include "Manager.h"
#include <utility>
#include <vector>
#include <nlohmann/json.hpp>

namespace tiny_swarm::manager {

    using Json = nlohmann::json;
    using PodGroup = std::vector<PodDescriptor *>;

    static Proxy::AddressPool getPodGroupAddressPool(const PodGroup &group) {
        Proxy::AddressPool pool;
        pool.reserve(group.size());

        for (auto &pod: group) {
            pool.emplace_back(pod->ip + ":" + std::to_string(pod->port_));
        }

        return pool;
    }

    enum ServiceStatus {
        prepare, ready, hung, close
    };

    /// Class ServiceDescriptor describes a service created by user.
    struct ServiceDescriptor {

        std::string token_;  // 用户service的唯一标识

        int num_;    // 用户预期的启动数量

        std::string proxy_address_; // 用户连接发送到这里，然后由代理进行转发

        ServiceStatus status; // 这个是用来处理 service 的特殊状态的量，例如一个服务需要多个 Pod，这些pod可能创建时会失败一部分，或者关闭时会失败一部分

        std::string service;
        std::vector<char *> exe_params_;
        std::vector<char *> docker_params_;
        ServiceType type_;
        int port_;
        int restart_;
        //bool transfer_{}; //是否允许迁移pod，这个transfer的逻辑可能要修改

    public:

        /// Constructor, RAII
        /// \param token Unique service token
        /// \param pods Owned pods
        /// \param num Reclaimed pod num
        /// \param proxy_address Proxy address to access, also called cluster ip
        /// \param manager Manager owned this descriptor
        explicit ServiceDescriptor(std::string token, PodGroup pods, int num, std::string proxy_address,
                                   Manager *manager)
                : token_(std::move(token)), pods_(std::move(pods)), num_(num),
                  proxy_address_(std::move(proxy_address)), manager_(manager) {

            status = pods_.size() == num_ ? ready : prepare;

            service = pods_[0]->service_;
            exe_params_ = pods_[0]->exe_params_;
            docker_params_ = pods_[0]->docker_params_;
            port_ = pods_[0]->port_;
            restart_ = pods_[0]->restart_;
            type_ = pods_[0]->type_;
        }

        /// Interface to simplify a stop operation. This function will stop all pods owned by this service group
        void stopPodsInService() {

            for (auto pod = pods_.begin(); pod != pods_.end();) {
                auto[res, error] = manager_->stopPod((*pod));
                if (res) {
                    pod = pods_.erase(pod);
                } else {
                    pod++;
                }
            }

            status = pods_.empty() ? close : hung;
        }

        /// Try to restart failed pods.
        /// \return The num of restarted pods
        int restartFailedPods() {

            if (pods_.size() == num_)
                return 0;

            size_t size = pods_.size();

            for (int i = 0; i < num_ - pods_.size(); i++) {

                auto alias = token_;

                alias += "_" + std::to_string(pods_.size() - 1);

                auto res = manager_->selectWorkerToCreatePod(service, alias, type_, port_,
                                                             exe_params_, docker_params_, restart_);

                if (!res.isFail()) {
                    pods_.emplace_back(res.podDescriptor());
                }
            }

            if (num_ == pods_.size())
                status = ready;

            return static_cast<int>(pods_.size() - size);
        }

        /// Get list of pod's address.
        /// \return A list with every pod's address
        Proxy::AddressPool getAddressPool() {
            return getPodGroupAddressPool(this->pods_);
        }

        /// Get the cluster ip of this service group
        /// \return Cluster ip
        Json getAccessAddress(){
            Json json;
            json["proxy_address"] = proxy_address_;

            std::vector<std::string> vec;
            vec.reserve(pods_.size());
            for(auto pod :pods_)
                vec.emplace_back( pod->ip + ":" + std::to_string(pod->port_));

            json["pods_address"] = vec;

            return json;
        }

        /// Generate message in json format
        /// \return generated message
        Json toJson() {
            Json json;
            json["token"] = token_;
            json["service"] = service;
            json["num"] = num_;
            json["proxy_address"] = proxy_address_;
            json["status"] = status;
            json["exe_param"] = exe_params_;
            json["docker_param"] = docker_params_;
            json["type"] = type_ == host ? "host" : "docker";

            std::vector<Json> vec;
            vec.reserve(pods_.size());

            auto pods = pods_;
            for(auto pod:pods)
                vec.emplace_back(pod->toJson());

            json["pod"] = vec;
            return json;
        }

        /// Not used now.
        /// \return
        std::string toSnapshot(){

            std::string snapshot = token_;

            snapshot += " " + std::to_string(num_);

            snapshot += " " + proxy_address_;

            snapshot += " " + std::to_string(status);

            snapshot += " {";

            for(auto &pod : pods_)
                snapshot += " " + pod->alias_ ;

            snapshot += " }";

            return snapshot;
        }

    private:

        Manager *manager_;
        PodGroup pods_;  // 这里存储的全部都是指针，最好不要直接访问存储的数据

    };

} // namespace manager

#endif //TINYSWARM_SERVICEDESCRIPTOR_H
