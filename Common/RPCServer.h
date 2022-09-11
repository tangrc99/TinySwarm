//
// Created by 唐仁初 on 2022/3/12.
//

#ifndef RPCSERVER_RPCSERVER_H
#define RPCSERVER_RPCSERVER_H

#include "TCPServer.h"
#include "MsgPack.pb.h"
#include "HTTPData.h"
#include <google/protobuf/service.h>
#include <net/if.h>
#include <sys/ioctl.h>

using google::protobuf::Service;
using google::protobuf::Message;

class RpcServerController : public google::protobuf::RpcController {
private:

    std::string error_text;
    bool failed;
    bool canceled;

public:
    ///Resets the RpcController to its initial state so that it may be reused in
    ///a new call.  Must not be called while an RPC is in progress
    void Reset() override {
        failed = false;
        canceled = false;
    }

    [[nodiscard]] std::string ErrorText() const override {
        return error_text;
    }

    [[nodiscard]] bool Failed() const override {
        return failed;
    }

    [[nodiscard]] bool IsCanceled() const override {
        return canceled;
    }

    void NotifyOnCancel(google::protobuf::Closure *callback) override {

    }

    void SetFailed(const std::string &reason) override {
        failed = true;
        error_text = reason;
    }

    void StartCancel() override {
        canceled = true;
    }

    RpcServerController() : canceled(false), failed(false), error_text({}) {}
};

class RPCServer {
private:
    TCPServer server;
    bool heart_beat = false;

    std::string local_address{};
    int port;

    std::string server_name;

    typedef std::map<std::string, Service *> ServiceMap;
    ServiceMap servMap;

    // 如何获得本地的 IP 地址
    static std::string getLocalIP() {
        int inet_sock;
        struct ifreq ifr{};
        char ip[32] = {'\0'};

        inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
        strcpy(ifr.ifr_name, "eth0");
        ioctl(inet_sock, SIOCGIFADDR, &ifr);
        strcpy(ip, inet_ntoa(((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr));

        if (strcmp(ip, "0.0.0.0") == 0) {
            inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
            strcpy(ifr.ifr_name, "en0");
            ioctl(inet_sock, SIOCGIFADDR, &ifr);
            strcpy(ip, inet_ntoa(((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr));
        }
        return {ip};
    }

public:
    explicit RPCServer(IPAddress addr, int thread_num = 2, int timeout = -1)
            : server(addr, new EventLoop(), thread_num, timeout), port(stoi(addr.getPort())) {

        server.setWriteCallBack(
                std::bind(&RPCServer::rpcWriteCallBack, this, std::placeholders::_1, std::placeholders::_2,
                          std::placeholders::_3));
        local_address = getLocalIP();
    }

    explicit RPCServer(IPAddress addr, std::vector<std::function<void()>> functors, int thread_num = 2,
                       int timeout = -1)
            : server(addr, new EventLoop(std::move(functors)), thread_num, timeout), port(stoi(addr.getPort())) {

        server.setWriteCallBack(
                std::bind(&RPCServer::rpcWriteCallBack, this, std::placeholders::_1, std::placeholders::_2,
                          std::placeholders::_3));
        local_address = getLocalIP();
    }

/// 从 fd 中 read 得到的 char *
/// 假设使用 HTTP 协议来传输
/// GET / HTTP/1.1\r\n
/// Method:
/// Input:
    void rpcWriteCallBack(const std::string &chs, size_t size, std::string &result) {

        if (chs.empty()) {

        }

        HTTPData httpData(chs, REQUEST);
        std::string pack = httpData.getHTTPBody();

        /// 从通讯包中提取到打包的信息
        MessagePack mp;
        mp.ParseFromString(pack);

        if (mp.IsInitialized()) {
            if (mp.data().empty()) {
                LOG_ERROR("%s", "Empty Data")
                HTTPData::makeResponseData(result, "ERROR : Empty Data");
                return;
            } else if (mp.service().empty()) {
                LOG_ERROR("%s", "Empty Service")
                HTTPData::makeResponseData(result, "ERROR : Empty Service");
                return;
            }

            LOG_DEBUG("%s: %s", "Calling service", &mp.service())
            Service *service = getService(mp.service());

            if (service == nullptr) {
                HTTPData::makeResponseData(result, "ERROR : Undefined Service");
            }

            LOG_DEBUG("%s: %s", "Calling method", &mp.method())
            auto method = service->GetDescriptor()->FindMethodByName(mp.method());

            if (method == nullptr) {
                HTTPData::makeResponseData(result, "ERROR : Undefined Method");
            }

            Message *data = service->GetRequestPrototype(method).New();

            LOG_DEBUG("%s: %s", "Pack Type", data->GetTypeName().c_str());

            if (data == nullptr) {
                HTTPData::makeResponseData(result, "ERROR : Undefined Packtype");
            }

            std::cout << "1" << std::endl;

            data->ParseFromString(mp.data());
            Message *echo = service->GetResponsePrototype(method).New();

            RpcServerController controller;

            std::cout << "2" << std::endl;

            service->CallMethod(method, &controller, data, echo, {});

            if (controller.Failed()) {
                LOG_DEBUG("%s: %s", "Rpc Fail", controller.ErrorText().c_str());
                HTTPData::makeResponseData(result, controller.ErrorText());
                return;
            }
            LOG_DEBUG("%s", "Rpc Success");

            auto http_res = HTTPData::makeResponseData(result, echo->SerializeAsString(), {});
            std::cout << echo->SerializeAsString().size();


        } else {
            HTTPData::makeResponseData(result, "ERROR : MessagePack Not Initialize");
            LOG_ERROR("%s", "MessagePack Not Initialize")
        }

    }

    Service *getService(const std::string &name) {
        auto it = servMap.find(name);

        return it == servMap.end() ? nullptr : it->second;
    }

    void addService(Service *service) {
        servMap.emplace(service->GetDescriptor()->name(), service);
    }

    void startService() {
        server.startListen();
    };

    ~RPCServer() {
        heart_beat = false;
    }

};


#endif //RPCSERVER_RPCSERVER_H
