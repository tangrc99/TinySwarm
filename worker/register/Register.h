//
// Created by 唐仁初 on 2022/9/3.
//

#ifndef TINYSWARM_REGISTER_H
#define TINYSWARM_REGISTER_H

#include <string>
#include <unordered_map>
#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>

namespace tiny_swarm::worker::registry {

    /// Class Register is used to register ip of worker to registry center. Lease is required to be used to make sure that
    /// manager will now connect to a down worker node.
    class Register {
    public:

        using ParamMap = std::unordered_map<std::string,std::string>;

        /// Constructor, no actions.
        Register() = default;

        /// Destructor, no actions.
        virtual ~Register() = default;

        /// Registry worker's name and information to registry center.
        /// \param name worker's name
        /// \param info worker's information, include uri, available services, hardware, etc.
        /// \return Is ok
        virtual bool registry(const std::string &name, const std::string &info) = 0;

        /// Generate registry information with required format.
        /// \param uri uri of worker
        /// \param services available services of worker
        /// \param status other status params of worker such as hardware, platform, etc.
        /// \return Formatted registry information.
        virtual std::string generateRegistryInfo(const std::string &uri,const ParamMap &services,const ParamMap &status) = 0;

        /// Check current lease status.
        /// \return Is lease ok
        virtual bool checkLeaseStatus() = 0;
    };

    /// Class ETCDRegister requires a etcd cluster as registry center. Registry information will use etcd' lease and
    /// automatically grants the lease. The information is stored by json format.
    class ETCDRegister : public Register {
    public:

        /// Establish connection to etcd cluster and get lease.
        /// \param url Uri of etcd cluster
        /// \param ttl Lease alive seconds, default 5
        explicit ETCDRegister(const std::string &url,int ttl = 5);

        /// Revoke owned etcd lease and connection.
        ~ETCDRegister() override {
            kp_->Cancel();
        }

        /// Registry to a etcd cluster. The operation will create a node "/tinyswarm/workers/this_worker_name", the value
        /// of this k-v pair stores json formatted information.
        /// \param name Name of this node, requires no same name between different worker node.
        /// \param info Json formatted information.
        /// \return Is ok
        bool registry(const std::string &name, const std::string &info) override;

        /// Generate json formatted information. For example,
        /// {
        /// "uri": "127.0.0.1:8888",\n
        ///     "services": {\n
        ///         "yolo": "1.1"\n
        ///     },\n
        ///     "status": {\n
        ///         "os": "ubuntu",\n
        ///         "gpu": "GTX3090"\n
        ///     }\n
        /// }\n
        /// \param uri uri of worker
        /// \param services available services of worker
        /// \param status other status params of worker such as hardware, platform, etc.
        /// \return Formatted registry information.
        std::string generateRegistryInfo(const std::string &uri,const ParamMap &services,const ParamMap &status) override;

        /// Check current lease status. Check action is using async call of etcd::KeepAlive, this function will check result.
        /// \return Is lease ok
        bool checkLeaseStatus() override;

    private:

    private:

        std::string path_in_etcd;   // 记录当前的根目录
        int64_t index_ = 0; // 存储在 etcd 中根目录下的 index 用来判断归属
        std::shared_ptr<etcd::KeepAlive> kp_;   // 自动续约操作
        etcd::Client client_;   // etcd client
        int ttl_;
        bool expired_ = false;  // lease 状态
    };

}


#endif //TINYSWARM_REGISTER_H
