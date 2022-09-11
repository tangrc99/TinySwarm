//
// Created by 唐仁初 on 2022/8/25.
//

#ifndef TINYSWARM_SESSION_H
#define TINYSWARM_SESSION_H

#include "IPAddress.h"
#include "RpcClientController.h"
#include "RpcClientChannel.h"
#include "RpcResult.h"
#include "DynamicGenerator.h"
#include <google/protobuf/service.h>
#include <thread>
#include <utility>
#include <unistd.h>
#include <google/protobuf/message.h>

#define RPC_AGAIN "AGAIN"
#define RPC_TIMEOUT "TIMEOUT"


class RpcAsyncSession final {
public:

    explicit RpcAsyncSession(std::shared_ptr<RpcClientChannel> channel) : channel_(std::move(channel)) {

    }

    ~RpcAsyncSession() {
        if (th_.joinable())
            th_.join();
    }

    /// Create an async rpc. There is only one async rpc running.
    /// \param method The method descriptor of rpc.
    /// \param req The input of rpc.
    /// \param res The output of rpc.
    /// \return If rpc task created. If failed, there is already one rpc running.
    bool run(const MethodDescriptor *method, const Message *req, MessagePtr res) {
        if (!isFinished())
            return false;

        tp = time(nullptr);     // 记录本次开始调用的时间

        running.test_and_set(); // 因为线程准备需要消耗资源，所以要在线程准备前声明

        response = std::move(res);

        th_ = std::thread([this, method, req]() {

            channel_->CallMethod(method, &controller, req, response.get(), {});

            running.clear();
        });

        return true;
    }

    /// Get the rpc response. Do call "isFinished" before call this function.
    /// \return The ptr of rpc response
    MessagePtr getResponse() {
        return response;
    }

    /// Check if rpc is finished.
    /// \return If rpc is finished.
    bool isFinished() {
        return !running.test();     // 如果正在运行，则无法取到结果
    }

    /// Check if rpc is failed.
    /// \return If rpc is failed.
    bool isFailed() {
        while (!isFinished()) {
            usleep(100);
        }
        if (controller.Failed())
            response = nullptr; // 清除本次调用的信息

        return controller.Failed();
    }

    /// Get fail reason if rpc is failed.
    /// \return Fail reason.
    std::string failedReason() {
        return controller.ErrorText();
    }



protected:
    std::shared_ptr<RpcClientChannel> channel_;

    MessagePtr response = nullptr;

    std::thread th_;
    std::atomic_flag running{};

    RpcClientController controller;

    time_t tp = 0;
    time_t max_wait = 5;
};


class RpcClient;

// rpc channel 的生命周期并不是由 Session 来控制
// 一个rpc channel 上可以建立多个 session，session 直接会公用同一个 tpc 链接。
// 多个 session 之间不共用 rpc controller, 同步与异步之间也不共用。
class Session {
public:

    /// Create a session on a connected rpc channel
    /// \param channel Connected rpc channel
    explicit Session(const std::shared_ptr<RpcClientChannel> &channel,
                     const google::protobuf::ServiceDescriptor *service,
                     std::shared_ptr<DynamicGenerator> generator,
                     RpcClient *client)
            : channel_(channel), timestamp(time(nullptr)), service_(service), generator_(std::move(generator)),
              cli_(client) {
        async_ = std::make_unique<RpcAsyncSession>(channel);
        channel_->sessionCreateNotify();
    }

    /// Copy Constructor. Create a new Session Object on original channel and client.
    /// \param other Session Object.
    Session(const Session &other) : channel_(other.channel_), timestamp(time(nullptr)), service_(other.service_),
                                    generator_(other.generator_), cli_(other.cli_) {
        async_ = std::make_unique<RpcAsyncSession>(channel_);
        channel_->sessionCreateNotify();
    }

    virtual ~Session() {
        channel_->sessionDestroyNotify();
    }

    bool connected() {
        return channel_->isConnected();
    }

    bool reconnect() {
        return channel_->reconnect();
    }

    RpcResult run(const std::string &method, const Message *input) {
        auto method_ = service_->FindMethodByName(method);
        return run(method_, input);
    }

    RpcResult run(const google::protobuf::MethodDescriptor *method, const Message *input) {

        if (method == nullptr)
            return RpcResult("Empty Method");

        MessagePtr output = MessagePtr(generator_->getMethodOutputProto(method)->New());

        std::cout << "output " << output->GetTypeName() << std::endl;

        channel_->CallMethod(method, &controller_, input, output.get(), {});

        if (controller_.Failed())
            return RpcResult(controller_.ErrorText());

        timestamp = time(nullptr);

        return RpcResult(output);
    }

    bool async_run(const std::string &method, const Message *input) {
        const MethodDescriptor *method_ = service_->FindMethodByName(method);
        return async_run(method_, input);
    }

    bool async_run(const google::protobuf::MethodDescriptor *method, const Message *input) {

        timestamp = time(nullptr);

        auto output = generator_->getMethodOutputProto(method);

        return async_->run(method, input, output);
    }

    /// Get async rpc result. Default lock.
    /// \param block Whether using block method. When using nonblock, return Error "Again" if rpc is unfinished.
    /// \return The result of rpc.
    RpcResult getAsyncResponse(bool block = true) {

        if (!block && !async_->isFinished()) {
            return RpcResult(RPC_AGAIN);
        }

        while (!async_->isFinished()) {
            usleep(5);  // 短暂休息
        }

        if (async_->isFailed())
            return RpcResult(async_->failedReason());

        timestamp = time(nullptr);
        return RpcResult(async_->getResponse());
    }

    /// Last rpc time. Only function run and async_run of class Session will update last rpc time value.
    /// \return Last rpc time.
    [[nodiscard]] time_t lastRpcTime() const {
        return timestamp;
    }

    IPAddress ipAddress() {
        return channel_->ipAddress();
    }

    std::string type() {
        return service_->name();
    }

protected:

    time_t timestamp; // 上次成功进行rpc的时间

    const google::protobuf::ServiceDescriptor *service_;

    RpcClient *cli_;    // Session 从属的 client

private:
    std::shared_ptr<RpcClientChannel> channel_; // 通信使用的channel
    std::unique_ptr<RpcAsyncSession> async_;    // 异步通信使用的 async 处理器
    RpcClientController controller_;    // 同步rpc的异常处理器
    std::shared_ptr<DynamicGenerator> generator_;   // 用于生成输出信息

};

#endif //TINYSWARM_SESSION_H
