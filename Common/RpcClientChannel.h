//
// Created by 唐仁初 on 2021/10/28.
//

#ifndef TCPTEST_RPCCLIENTCHANNEL_H
#define TCPTEST_RPCCLIENTCHANNEL_H

#include "IPAddress.h"
#include "MsgPack.pb.h"
#include "HTTPData.h"
#include "HTTPParser.h"
#include "Buffer.h"

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <iostream>
#include <unistd.h>
#include <google/protobuf/message.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>
#include <sw/redis++/redis.h>
#include <memory>
#include <utility>


using google::protobuf::RpcChannel;
using google::protobuf::Message;
using google::protobuf::MethodDescriptor;
using google::protobuf::RpcController;


/// Class RpcClientChannel is based on Class google::protobuf::RpcChannel, provides serialize and web socket
class RpcClientChannel : public google::protobuf::RpcChannel {
public:


    explicit RpcClientChannel(IPAddress ipAddress) : address(ipAddress), sockfd(::socket(AF_INET, SOCK_STREAM, 0)),
                                                     sessions_(0) {

//        // non-block
//        int flags = ::fcntl(sockfd, F_GETFL, 0);  ///得到文件状态标记
//        flags |= O_NONBLOCK;
//        int ret = ::fcntl(sockfd, F_SETFL, flags);    ///设置文件状态标记
//        (void) ret;

        auto res = ::connect(sockfd, address.toSockAddrPtr(), address.addrlen());

        if (0 != res) {
            setDisconnected();
            std::cerr << "Socket Disconnected!\n";
            return;
        }        //exit(1);
        connected = true;
        std::cout << "Socket Connected!\n";
    }


    ~RpcClientChannel() override {
        ::close(sockfd);
    }

    /// Try to reconnect peer if channel is down.
    /// \return If reconnect succeed.
    bool reconnect() {

        // 被系统检测到关闭的 socket_fd 不能再重复使用
        sockfd = ::socket(AF_INET, SOCK_STREAM, 0);

        if (0 != ::connect(sockfd, address.toSockAddrPtr(), address.addrlen())) {
            setDisconnected();
        } else
            connected = true;

        return connected;
    }


    /// Check if channel is connected.
    /// \return Is connected
    [[nodiscard]] bool isConnected() const {
        return connected;
    }

    /// @note Every new session on channel should call this function
    void sessionCreateNotify() {
        sessions_.fetch_add(1);
    }

    /// @note Every session should call this function when call its destruction.
    void sessionDestroyNotify() {
        sessions_.fetch_sub(1);
    }

    /// Get the nums of sessions on this channel, thread safety.
    /// \return The num of sessions
    [[nodiscard]] int sessions() const {
        return sessions_.load();
    }

    /// Get the ipAddress of this rpc channel
    /// \return IPAddress
    IPAddress ipAddress() {
        return address;
    }

    /// Get last rpc called timestamp of this rpc channel
    /// \return Last rpc timestamp
    [[nodiscard]] time_t getLastTimestamp() const {
        return timestamp;
    }

    /// Call rpc method, this function is based on protobuf rpc.
    /// \param method The method to be called
    /// \param controller The rpc controller
    /// \param request The rpc input
    /// \param response The rpc output
    /// \param done Not used
    void CallMethod(const MethodDescriptor *method,
                    google::protobuf::RpcController *controller, const google::protobuf::Message *request,
                    google::protobuf::Message *response, google::protobuf::Closure *done) override {

        std::lock_guard<std::mutex> lg(mtx);

        if (!connected) {
            controller->SetFailed("Not Connect");
            return;
        }

        controller->Reset();


        /// 需要实现数据的封装和通信过程
        /// 按照method: echo 的方式进行封装
        MessagePack pack;

        try {

            pack.set_data(request->SerializeAsString());    /// 将序列化后的数据打包
            assert(!pack.data().empty());
            pack.set_packtype(request->GetTypeName());  /// 记录打包前的数据类型
            pack.set_method(method->name());    /// 需要调用的方法
            pack.set_service(method->service()->name());    /// 需要调用的方法所在的服务

        } catch (std::exception &e) {
            controller->SetFailed("Message Pack Error");
        }


        std::string sss = pack.SerializeAsString();
        //std::cout << sss << std::endl;

        try {
            sendMessages(sss);
            waitResponse(response);
            timestamp = time(nullptr);

        } catch (std::exception &e) {
            controller->SetFailed(e.what());
        }

    }

private:

    static int alarm_flag;

    std::atomic<int> sessions_; // the num of sessions

    time_t timestamp{}; // 上一次成功通信的时间

    IPAddress address;
    int sockfd = 0;
    bool connected = false;

    std::mutex mtx;

    void sendMessages(const std::string &str) {
        std::string str_to_send;

        HTTPParser::getReqMessage(str_to_send, {"POST", "", "1.1"}, {{"1", "1"}}, str);

//        std::cout << "write" <<
//                  ::write(sockfd, str_to_send.c_str(), str_to_send.size());

        auto total = str_to_send.size();
        size_t pos = 0;

        while (pos < total) {
            pos += ::write(sockfd, str_to_send.substr(pos).c_str(), total - pos);

            if (pos == -1) {
                setDisconnected();
                std::cerr << errno << std::endl;
                throw std::runtime_error("Write Error " + std::to_string(errno));
            }
        }

    }

    static void Alarm(int) {
        alarm_flag = 1;
    }

    void waitResponse(google::protobuf::Message *response) {

        std::cout << "waiting for response\n";
        std::cout << "-------------\n";

        Buffer buffer;

        size_t rd;

        // 定时三十秒，防止读操作一直被阻塞
        signal(SIGALRM, Alarm);
        alarm_flag = 0;
        alarm(30);

        // 读取数据
        do {

            rd = buffer.readToBuffer(sockfd);   // 如果数据还没到，这里会阻塞

            std::cout << "wait" << rd << std::endl;

        } while (!buffer.isCatchHTTPEnd() && rd > 0);

        // 如果长时间没有收到回复
        if (alarm_flag == 1) {
            setDisconnected();
            throw std::runtime_error("RPC TIMEOUT");
        }

        std::cout << "end wait" << std::endl;

        // 判断数据包是否是完整的
        if (!buffer.isCatchHTTPEnd()) {

            if (rd == 0) {
                setDisconnected();
                throw std::runtime_error("DisConnected");
            } else {

                throw std::runtime_error("Message Error");
            }

        }

        auto str = buffer.str();

        HTTPData HTTPdata(buffer.str(), HTTP_TYPE::RESPONSE);

        auto rpc_pack = HTTPdata.getParamValue("RPCRESPONSE");

        response->ParseFromString(rpc_pack);


        std::string space = " ";
        ::write(sockfd, space.c_str(), 1);
    }

    /// Set this channel disconnected.
    void setDisconnected() {
        connected = false;
        ::close(sockfd);
    }

};


#endif //TCPTEST_RPCCLIENTCHANNEL_H
