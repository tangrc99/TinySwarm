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
//using namespace google::protobuf::compiler;

class RpcClientChannel : public google::protobuf::RpcChannel {
private:

    std::atomic<int> sessions_; // the num of sessions

    time_t timestamp{}; // 上一次成功通信的时间

    IPAddress address;
    int sockfd = 0;
    bool connected = false;

    std::mutex mtx;

    void sendMessages(const std::string &str) const {
        std::string str_to_send;

        HTTPParser::getReqMessage(str_to_send, {"POST", "", "1.1"}, {{"1", "1"}}, str);

//        std::cout << "write" <<
//                  ::write(sockfd, str_to_send.c_str(), str_to_send.size());

        auto total = str_to_send.size();
        size_t pos = 0;

        while (pos < total) {
            pos += ::write(sockfd, str_to_send.substr(pos).c_str(), total - pos);

            if (pos == -1) {
                std::cerr << errno << std::endl;
                throw std::runtime_error("Write Error " + std::to_string(errno));
            }
        }

    }

    void waitResponse(google::protobuf::Message *response) const {

        std::cout << "waiting for response\n";
        std::cout << "-------------\n";

        Buffer buffer;

        size_t rd;

        do {

            rd = buffer.readToBuffer(sockfd);   // 如果数据还没到，这里会阻塞

        } while (!buffer.isCatchHTTPEnd() && rd > 0);

        if (!buffer.isCatchHTTPEnd()) {

            if (rd == 0)
                throw std::runtime_error("No Messages Back");
            else
                throw std::runtime_error("not catch http end");

        }

        auto str = buffer.str();

        HTTPData HTTPdata(buffer.str(), HTTP_TYPE::RESPONSE);

        auto rpc_pack = HTTPdata.getParamValue("RPCRESPONSE");

        response->ParseFromString(rpc_pack);


        std::string space = " ";
        ::write(sockfd, space.c_str(), 1);

    }


public:

    explicit RpcClientChannel(IPAddress ipAddress) : address(ipAddress), sockfd(::socket(AF_INET, SOCK_STREAM, 0)),
                                                     sessions_(0) {

        auto res = ::connect(sockfd, address.toSockAddrPtr(), address.addrlen());

        if (0 != res) {
            connected = false;
            std::cerr << "Socket Disconnected!\n";
            return;
        }        //exit(1);
        connected = true;
        std::cout << "Socket Connected!\n";
    }

    bool reconnect() {
        if (0 != ::connect(sockfd, address.toSockAddrPtr(), address.addrlen())) {
            connected = false;
        }        //exit(1);
        connected = true;

        return connected;
    }

    ~RpcClientChannel() override {
        ::close(sockfd);
    }

    [[nodiscard]] bool isConnected() const {
        return connected;
    }

    // Every new session on channel should call this function
    void sessionCreateNotify() {
        sessions_.fetch_add(1);
    }

    void sessionDestroyNotify() {
        sessions_.fetch_sub(1);
    }

    [[nodiscard]] int sessions() const {
        return sessions_.load();
    }

    IPAddress ipAddress() {
        return address;
    }

    [[nodiscard]] time_t getLastTimestamp() const{
        return timestamp;
    }

    // 同步调用:传入 response 指针后会阻塞等待结果的返回
    // 异步调用:传入 response 指针后，对象不会立刻构造，调用 getAsyncRes 来获取结果
    // FIXME::内部需要进行错误处理，利用 controller 将错误传递出去
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

        try{

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

};


#endif //TCPTEST_RPCCLIENTCHANNEL_H
