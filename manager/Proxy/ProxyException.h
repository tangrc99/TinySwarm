//
// Created by 唐仁初 on 2022/9/4.
//

#ifndef PROXY_PROXYEXCEPTION_H
#define PROXY_PROXYEXCEPTION_H

#include <exception>
#include <string>

namespace tiny_swarm::manager::proxy {
        /// Class ProxyException is used in Proxy classes to identify where exception was threw.
        class ProxyException : public std::exception {
        public:

            explicit ProxyException(const char *reason) noexcept: what_(reason) {}

            explicit ProxyException(const std::string &reason) noexcept: what_(reason.c_str()) {}

            ProxyException(const ProxyException &) noexcept = default;

            ~ProxyException() noexcept override = default;

            [[nodiscard]] const char *what() const noexcept override {
                return what_;
            };

        private:
            const char *what_;
        };
    }

#endif //PROXY_PROXYEXCEPTION_H
