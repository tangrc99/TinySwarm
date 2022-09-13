//
// Created by trc on 2021/10/18.
//

#ifndef TCPTEST_IPADDRESS_H
#define TCPTEST_IPADDRESS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <typeinfo>
#include <cstring>
#include <string>
#include <vector>


class IPAddress {
private:
    union {
        sockaddr_in ipv4;
        sockaddr_in6 ipv6;
    } address{};

    enum TYPE {
        IPV4, IPV6
    } type;

public:

    [[nodiscard]] sockaddr *toSockAddrPtr() const {
        return (sockaddr *) &address;
    }

    explicit IPAddress(sockaddr_in addr) : type(IPV4) {
        address.ipv4 = addr;
    };

    explicit IPAddress(sockaddr_in6 addr6) : type(IPV6) {
        address.ipv6 = addr6;
    };

    explicit IPAddress(int fd) {
        sockaddr_in addr{};
        socklen_t len = sizeof(addr);
        getpeername(fd, (sockaddr *) &addr, &len);

        type = IPV4;
        address.ipv4 = addr;
    }

    /// 创建一个服务器的 IP
    explicit IPAddress(int af, unsigned short port) {
        if (af == AF_INET) {
            bzero(&address.ipv4, sizeof(address.ipv4));
            address.ipv4.sin_addr.s_addr = htonl(INADDR_ANY);
            address.ipv4.sin_family = AF_INET;
            address.ipv4.sin_port = htons(port);
            type = IPV4;
        } else if (af == AF_INET6) {
            bzero(&address.ipv6, sizeof(address.ipv6));
            address.ipv6.sin6_addr = in6addr_any;
            address.ipv6.sin6_family = AF_INET6;
            address.ipv6.sin6_port = htons(port);
            type = IPV6;
        }
    }

    /// 将字符串转化为 IPAddress 类,字符串为空默认使用本机地址 0.0.0.0。
    explicit IPAddress(const char *addr, int af, unsigned short port) {
        if (af == AF_INET) {
            bzero(&address.ipv4, sizeof(address.ipv4));
            inet_pton(AF_INET, addr, &address.ipv4.sin_addr);
            address.ipv4.sin_family = AF_INET;
            address.ipv4.sin_port = htons(port);
            type = IPV4;
        } else if (af == AF_INET6) {
            bzero(&address.ipv6, sizeof(address.ipv6));
            address.ipv6.sin6_addr = in6addr_any;
            inet_pton(AF_INET6, addr, &address.ipv6.sin6_addr);

            address.ipv6.sin6_family = AF_INET6;
            address.ipv6.sin6_port = htons(port);
            type = IPV6;
        }
    }

    explicit IPAddress(const char *addr) : IPAddress(std::string(addr)) {

    }

    explicit IPAddress(const std::string &addr, int af = AF_INET) {

        auto pos = addr.find(':');
        auto ip = addr.substr(0, pos);
        auto port = addr.substr(pos + 1);

        if (af == AF_INET) {
            bzero(&address.ipv4, sizeof(address.ipv4));
            inet_pton(AF_INET, ip.c_str(), &address.ipv4.sin_addr);
            address.ipv4.sin_family = AF_INET;
            address.ipv4.sin_port = htons(stoi(port));
            type = IPV4;
        } else if (af == AF_INET6) {
            bzero(&address.ipv6, sizeof(address.ipv6));
            address.ipv6.sin6_addr = in6addr_any;
            inet_pton(AF_INET6, ip.c_str(), &address.ipv6.sin6_addr);
            address.ipv6.sin6_family = AF_INET6;
            address.ipv6.sin6_port = htons(stoi(port));
            type = IPV6;
        }

    }

    IPAddress(sockaddr addr, socklen_t len) {
        if (len == sizeof(sockaddr_in)) {
            address.ipv4 = *(sockaddr_in *) &addr;
            type = IPV4;
        } else if (len == sizeof(sockaddr_in6)) {
            address.ipv6 = *(sockaddr_in6 *) &addr;
            type = IPV6;
        }

    }

    static std::vector<std::string> getIpByHost(const std::string &hostname) {
        std::vector<std::string> result;
        auto host = gethostbyname(hostname.c_str());
        for (int i = 0; host->h_addr_list[i]; i++) {
            result.emplace_back(inet_ntoa(*(in_addr *) host->h_addr_list[i]));
        }
        return result;
    }

    [[nodiscard]] socklen_t addrlen() const {
        return type == IPV4 ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
    }

    [[nodiscard]] int famyly_t() const {
        return type == IPV4 ? AF_INET : AF_INET6;
    }

    [[nodiscard]] std::string getAddress() const {
        if (type == IPV4) {
            std::string addr = inet_ntoa(address.ipv4.sin_addr);
            addr += ":";
            addr += std::to_string(ntohs(address.ipv4.sin_port));
            return addr;
        }
        return {};
    }

    [[nodiscard]] std::string getAddressWithoutPort() const {
        if (type == IPV4) {
            std::string addr = inet_ntoa(address.ipv4.sin_addr);
            return addr;
        }
        return {};
    }

    [[nodiscard]] std::string getPort() const {
        if (type == IPV4) {
            return std::to_string(ntohs(address.ipv4.sin_port));
        }
        return {};
    }

    bool operator==(const IPAddress &other) const {
        return this->getAddress() == other.getAddress();
    }
};


#endif //TCPTEST_IPADDRESS_H
