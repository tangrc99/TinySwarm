//
// Created by 唐仁初 on 2022/9/4.
//

#ifndef PROXY_PROXY_H
#define PROXY_PROXY_H

#include "ProxyException.h"
#include <vector>
#include <string>

namespace manager {

    /// 要求底层的负载均衡机制必须为轮询，不能为ip hash，
    class Proxy {
    public:

        using AddressPool = std::vector<const std::string>;

        virtual std::string insertAddressPool(const std::string &pool_name, const AddressPool &pool) = 0;

        virtual bool deleteAddressPool(const std::string &pool_name) = 0;

        virtual bool updateAddressPool(const std::string &pool_name, const AddressPool &pool) = 0;

        virtual AddressPool findAddressPool(const std::string &pool_name) = 0;

        virtual int getBindPort(const std::string &pool_name) = 0;

        [[nodiscard]] virtual std::string proxyType() const = 0;

    private:

    };
}

#endif //PROXY_PROXY_H
