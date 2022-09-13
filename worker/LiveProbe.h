//
// Created by 唐仁初 on 2022/9/9.
//

#ifndef WORKER_LIVEPROBE_H
#define WORKER_LIVEPROBE_H

#include <unistd.h>
#include "IPAddress.h"

namespace worker {

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

        [[nodiscard]] time_t lastProbeTime() const {
            return probe_time_;
        }

    protected:
        time_t probe_time_ = 0;
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
            int fd = ::socket(AF_INET, SOCK_STREAM, 0);
            int res = ::connect(fd, ipAddress.toSockAddrPtr(), ipAddress.addrlen());
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
