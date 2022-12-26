//
// Created by 唐仁初 on 2022/9/9.
//

#ifndef WORKER_LIVEPROBE_H
#define WORKER_LIVEPROBE_H

#include <unistd.h>
#include <sys/fcntl.h>
#include "IPAddress.h"

namespace worker::probe {

/// Class LiveProbe is used to check whether the process of pod is attachable.
    class LiveProbe {
    public:

        /// Construction of Class LiveProbe
        LiveProbe() = default;

        /// Destruction of Class LiveProbe
        virtual ~LiveProbe() = default;

        /// Check if ip is attachable and alive.
        /// \param ipAddress The address to be detected
        /// \return Is ip attachable
        virtual bool isPeerAlive(IPAddress ipAddress) {
            return true;
        }

        /// Get last probed time. This function is used to control probe frequency.
        /// \return Last probe time
        [[nodiscard]] time_t lastProbeTime() const {
            return probe_time_;
        }

    protected:

        time_t probe_time_ = time(nullptr); // 最近一次探测的时间
    };

/// Class TCPProbe is used to check whether the process of pod is attachable.
/// The Probe will use tcp to check.
/// @note If pod is using reactor model, tcp probe can only detect if listen channel is blocked.
    class TCPProbe : public LiveProbe {
    public:

        /// Construction of Class TCPProbe
        TCPProbe() = default;

        /// Destruction of Class TCPProbe
        ~TCPProbe() override = default;

        /// Check if ip is alive using tcp socket. If a tcp connection is accepted, the peer address is alive
        /// \param ipAddress The address to be detected
        /// \return Is ip attachable
        bool isPeerAlive(IPAddress ipAddress) override {

            // 更新探测时间
            probe_time_ = time(nullptr);

            int fd = ::socket(AF_INET, SOCK_STREAM, 0);

            // 将 fd 设置为非阻塞状态
            int flags = ::fcntl(fd, F_GETFL, 0);  // 得到文件状态标记
            flags |= O_NONBLOCK;
            int ret = ::fcntl(fd, F_SETFL, flags);    // 设置文件状态标记
            (void) ret;

            int res = ::connect(fd, ipAddress.toSockAddrPtr(), ipAddress.addrlen());

            if(res < 0 && errno != EINPROGRESS) {
                ::close(fd);
                return false;
            }

            ::close(fd);
            return res == 0;
        }

    };

/// Class TCPProbe is used to check whether the process of pod is attachable.
/// The Probe will use tcp to check.
/// @note If pod is using reactor model, http probe can assert if listen channel and one of worker channel is not blocked.
    class HTTPProbe : public LiveProbe {
    public:

        HTTPProbe() = default;

        ~HTTPProbe() override = default;

        bool isPeerAlive(IPAddress ipAddress) override {

            return true;
        }
    };
}
#endif //WORKER_LIVEPROBE_H
