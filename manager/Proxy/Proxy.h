//
// Created by 唐仁初 on 2022/9/4.
//

#ifndef PROXY_PROXY_H
#define PROXY_PROXY_H

#include "ProxyException.h"
#include <vector>
#include <string>

namespace manager::proxy{

        /// Class Proxy defines all interface to construct a service gateway
        class Proxy {
        public:

            /// Every address is host:port.
            using AddressPool = std::vector<const std::string>;

            virtual ~Proxy() = default;

            /// Interface to add a proxy pool.
            /// \param pool_name Proxy pool name. Typically a service name or pod name.
            /// \param pool Proxy address. Typically pods' address.
            /// \return Proxy pool name.
            virtual std::string insertAddressPool(const std::string &pool_name, const AddressPool &pool) = 0;

            /// Interface to delete a proxy pool.
            /// \param pool_name Proxy pool name. Typically a service name or pod name.
            /// \return Is ok
            virtual bool deleteAddressPool(const std::string &pool_name) = 0;

            /// Interface to update a proxy pool.
            /// \param pool_name Proxy pool name. Typically a service name or pod name.
            /// \param pool Proxy address. Typically pods' address.
            /// \return Proxy pool name.
            virtual bool updateAddressPool(const std::string &pool_name, const AddressPool &pool) = 0;

            /// Get proxy address by name.
            /// \param pool_name Proxy pool name. Typically a service name or pod name.
            /// \return Proxy address. Typically pods' address.
            virtual AddressPool findAddressPool(const std::string &pool_name) = 0;

            /// Get proxy type.
            /// \return Proxy type name
            [[nodiscard]] virtual std::string proxyType() const = 0;

        private:
            /// Not Used now.
            /// \param pool_name
            /// \return
            virtual int getBindPort(const std::string &pool_name) = 0;
        };
    }

#endif //PROXY_PROXY_H
