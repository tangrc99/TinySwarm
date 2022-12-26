//
// Created by 唐仁初 on 2022/11/8.
//

#ifndef TINYSWARM_IPVSPROXY_H
#define TINYSWARM_IPVSPROXY_H

#include "Proxy.h"
#include <unordered_map>
#include <mutex>

namespace tiny_swarm::manager::proxy {
    /// Class IpvsProxy uses iptables and ipvs tool to manage client's route path to pods. Every service group will use
    /// a virtual ip 10.0.1.x:x to create a virtual server. The virtual server will use round-robin strategy to transmit
    /// user's connection. User can also use real ip to visit a single server.
    class IpvsProxy : public Proxy {
    public:

        IpvsProxy() {

            auto sys_res = system("route add -net 10.0.1.0 netmask 255.255.255.0 gw localhost");
            if (sys_res != 0)
                throw ProxyException("Update Route Error");

            system("ipset create CLUSTER_IP hash:ip,port");
            system("iptables -t mangle -m set --match-set CLUSTER_IP -j MARK --set-mark 200");
        }

        ~IpvsProxy() override = default;

        /// Interface to add a proxy pool.
        /// \param pool_name Proxy pool name. Typically a service name or pod name.
        /// \param pool Proxy address. Typically pods' address.
        /// \return Proxy pool name.
        std::string insertAddressPool(const std::string &pool_name, const AddressPool &pool) override {

            auto server = new IpvsServer({updateAddress(), pool});
            pools_.emplace(pool_name, server);

            std::string command = "ipvsadm -A -t ";
            command += server->vs_address_ + " -s rr";

            if (system(command.c_str()) != 0) {
                throw ProxyException("IPVS -A ERROR");
            }

            for (auto &rs: server->rs_address_) {
                command = "ipvsadm -a -t ";
                command += rs + "-m";
                if (system(command.c_str()) != 0) {
                    throw ProxyException("IPVS -a ERROR");
                }
            }

            // 加入 CLUSTER_IP
            command = "ipset add CLUSTER_IP hash:"; // FIXME: 这里有一个 ,
            command += server->vs_address_;
            if (system(command.c_str()) != 0) {
                throw ProxyException("IPSET add ERROR");
            }
            // 加入 NODE_PORT

            return server->vs_address_;
        }

        /// Interface to delete a proxy pool.
        /// \param pool_name Proxy pool name. Typically a service name or pod name.
        /// \return Is ok
        bool deleteAddressPool(const std::string &pool_name) override {
            auto pos = pools_.find(pool_name);
            if (pos == pools_.end())
                return false;
            auto server = pos->second;

            std::string command = "ipvsadm -D -t ";
            command += server->vs_address_ + " -s rr";

            if (system(command.c_str()) != 0) {
                throw ProxyException("IPVS -D ERROR");
            }

            // 删除 CLUSTER_IP
            command = "ipset del CLUSTER_IP hash:"; // FIXME: 这里有一个 ,
            command += server->vs_address_;
            if (system(command.c_str()) != 0) {
                throw ProxyException("IPSET del ERROR");
            }
            // 加入 NODE_PORT
            return true;
        }

        /// Interface to update a proxy pool.
        /// \param pool_name Proxy pool name. Typically a service name or pod name.
        /// \param pool Proxy address. Typically pods' address.
        /// \return Proxy pool name.
        bool updateAddressPool(const std::string &pool_name, const AddressPool &pool) override {
            auto pos = pools_.find(pool_name);
            if (pos == pools_.end())
                return false;
            deleteAddressPool(pool_name);
            return !insertAddressPool(pool_name, pool).empty();
        }

        /// Get proxy address by name.
        /// \param pool_name Proxy pool name. Typically a service name or pod name.
        /// \return Proxy address. Typically pods' address.
        AddressPool findAddressPool(const std::string &pool_name) override {
            auto pos = pools_.find(pool_name);
            if (pos == pools_.end())
                return {};
            return pos->second->rs_address_;
        }

        /// Get proxy type.
        /// \return Proxy type name
        [[nodiscard]] std::string proxyType() const override {
            return "IPVS";
        }

    private:

        /// Not Used
        /// \param pool_name
        /// \return
        int getBindPort(const std::string &pool_name) override {
            return -1;
        }

        /// Get an unused virtual server address
        /// \return Virtual Server Address
        std::string updateAddress() {
            std::lock_guard<std::mutex> lg(mtx);
            if (assigned_port > 10000) {
                assigned_port = 8000;
                assigned_addr++;
            }
            // 目前只能够支持 255*2000 个虚拟服务地址
            if (assigned_addr >= 255)
                return "";
            return "10.0.1." + std::to_string(assigned_addr) + ":" + std::to_string(assigned_port);
        }

        struct IpvsServer {
            std::string vs_address_;    // virtual server
            AddressPool rs_address_;    // real servers
        };

        std::unordered_map<std::string, IpvsServer *> pools_;

        std::mutex mtx;
        int assigned_port = 8000;
        int assigned_addr = 1;
    };
}
#endif //TINYSWARM_IPVSPROXY_H
