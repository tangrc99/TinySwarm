//
// Created by 唐仁初 on 2022/7/21.
//

#ifndef RPCSERVER_RPCCLIENTCONTROLLER_H
#define RPCSERVER_RPCCLIENTCONTROLLER_H

#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>
#include <csignal>
#include <thread>


class ClosureImpl : public google::protobuf::Closure {
public:

    void Run() override{

    }

};


class RpcClientController : public google::protobuf::RpcController {
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
        error_text = {};
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
        if(canceled)    // 如果已经取消了，防止覆盖error_text
            return ;

        failed = true;
        error_text = reason;
    }

    void Cancel(std::thread &th){
        StartCancel();
        pthread_kill(th.native_handle(), SIGALRM);
    }

    void StartCancel() override {
        canceled = true;
        failed = true;
        error_text = "RPC TIMEOUT";
    }

    RpcClientController() : canceled(false), failed(false), error_text({}) {}
};

#endif //RPCSERVER_RPCCLIENTCONTROLLER_H
