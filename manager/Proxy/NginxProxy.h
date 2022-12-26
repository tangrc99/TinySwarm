//
// Created by 唐仁初 on 2022/9/4.
//

#ifndef PROXY_NGINXPROXY_H
#define PROXY_NGINXPROXY_H

#include "Proxy.h"

#include <unordered_map>
#include <filesystem>
#include <utility>
#include <fstream>

#include <iostream>

namespace manager::proxy {
        /// NginxProxy will build a service gateway at the application layer
        class NginxProxy : public Proxy {
        public:

            /// Construction to init nginx service.
            /// \param nginx_dir Nginx root directory
            explicit NginxProxy(std::string nginx_dir) : nginx_dir_(std::move(nginx_dir)) {
                nginx_exec_ = nginx_dir_;
                nginx_exec_.append("sbin").append("nginx");
                if (!exists(nginx_exec_)) {
                    throw ProxyException("Nginx Exec Not Found.");
                }

                nginx_conf_ = nginx_dir_;
                nginx_conf_.append("conf").append("proxy.conf");
                writeConfFile(basic_conf);

                std::string start = nginx_exec_.string() + " -c " + nginx_conf_.string();
                int res = system(start.c_str());

                if (res == -1)
                    throw std::runtime_error("Nginx Start Error");
            }

            /// Stop nginx process
            ~NginxProxy() override {

                std::string quit = nginx_exec_.string() + " -s stop";
                system(quit.c_str());
            }

            /// Interface to add a proxy pool.
            /// \param pool_name Proxy pool name. Typically a service name or pod name.
            /// \param pool Proxy address. Typically pods' address.
            /// \return Proxy pool name.
            std::string insertAddressPool(const std::string &pool_name, const AddressPool &pool) override {

                if (pools_.find(pool_name) != pools_.end())
                    return {};

                pools_.emplace(pool_name, std::make_pair(pool, port_));

                updateNginxStatus();

                return "127.0.0.1:" + std::to_string(port_++);
            }

            /// Interface to update a proxy pool.
            /// \param pool_name Proxy pool name. Typically a service name or pod name.
            /// \param pool Proxy address. Typically pods' address.
            /// \return Proxy pool name.
            bool updateAddressPool(const std::string &pool_name, const AddressPool &pool) override {
                if (pools_.find(pool_name) != pools_.end())
                    return false;

                int port = pools_[pool_name].second;

                pools_[pool_name] = {pool, port};

                return true;
            }

            /// Interface to delete a proxy pool.
            /// \param pool_name Proxy pool name. Typically a service name or pod name.
            /// \return Is ok
            bool deleteAddressPool(const std::string &pool_name) override {

                auto it = pools_.find(pool_name);
                if (it == pools_.end())
                    return false;

                pools_.erase(pool_name);
                return true;
            }

            /// Get proxy address by name.
            /// \param pool_name Proxy pool name. Typically a service name or pod name.
            /// \return Proxy address. Typically pods' address.
            AddressPool findAddressPool(const std::string &pool_name) override {
                auto it = pools_.find(pool_name);
                return it == pools_.end() ? AddressPool{} : it->second.first;
            }

            /// Not Used now.
            /// \param pool_name
            /// \return
            int getBindPort(const std::string &pool_name) override {
                auto it = pools_.find(pool_name);
                return it == pools_.end() ? -1 : it->second.second;
            }

            /// Get proxy type.
            /// \return Proxy type name
            [[nodiscard]] std::string proxyType() const override {
                return "Nginx";
            }


        private:

            void updateNginxStatus() {

                std::string conf_str = basic_conf;

                for (const auto &pool_: pools_) {
                    auto &name = pool_.first;
                    auto &pool = pool_.second.first;
                    auto &bind_port = pool_.second.second;
                    conf_str.insert(conf_str.size() - 1, toNginxConf(name, pool, bind_port));
                }

                writeConfFile(conf_str);

                std::string reload = nginx_exec_.string() + " -s reload";

                system(reload.c_str());
            }

            void writeConfFile(const std::string &content) {
                std::ofstream conf_os(nginx_conf_);
                conf_os << content;
            }

            [[nodiscard]] static std::string
            toNginxConf(const std::string &name, const AddressPool &pool, int bind_port) {
                std::string result;
                result.reserve(1000);


                result += "\tupstream stream_" + name + " {\n";
                for (auto &address: pool)
                    result += "\t\tserver " + address + " weight=1;\n";
                result += "\t}\n";
                result += "\tserver {\n"
                          "\t\tlisten " + std::to_string(bind_port) + ";\n"
                                                                      "\t\tproxy_pass stream_" + name + ";\n"
                                                                                                        "\t}\n";

                return result;
            }


        private:

            std::filesystem::path nginx_dir_, nginx_exec_, nginx_conf_;


            std::unordered_map<std::string, std::pair<AddressPool, int>> pools_;

            const std::string basic_conf = "\n"
                                           "#user  nobody;\n"
                                           "worker_processes  2;\n"
                                           "\n"
                                           "#error_log  logs/error.log;\n"
                                           "#error_log  logs/error.log  notice;\n"
                                           "#error_log  logs/error.log  info;\n"
                                           "\n"
                                           "#pid        logs/nginx.pid;\n"
                                           "\n"
                                           "\n"
                                           "events {\n"
                                           "    worker_connections 1024;\n"
                                           "}\n"
                                           "\n"
                                           "stream {\n"
                                           "}";

            int port_ = 10000;
        };

    }

#endif //PROXY_NGINXPROXY_H
