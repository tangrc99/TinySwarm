//
// Created by 唐仁初 on 2022/9/6.
//

#ifndef TINYSWARM_NOPROXY_H
#define TINYSWARM_NOPROXY_H

#include "Proxy.h"

namespace manager {
    class NoProxy final : public Proxy {
    public:

        std::string insertAddressPool(const std::string &pool_name, const AddressPool &pool) override {
            return {};
        }

        bool deleteAddressPool(const std::string &pool_name) override {
            return true;
        }

        bool updateAddressPool(const std::string &pool_name, const AddressPool &pool) override {
            return true;
        }

        AddressPool findAddressPool(const std::string &pool_name) override {
            return {};
        }

        int getBindPort(const std::string &pool_name) override {
            return -1;
        }

        [[nodiscard]] std::string proxyType() const override {
            return "NoProxy";
        }
    };
}


#endif //TINYSWARM_NOPROXY_H
