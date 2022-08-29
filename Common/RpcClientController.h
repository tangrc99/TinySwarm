//
// Created by 唐仁初 on 2022/7/21.
//

#ifndef RPCSERVER_RPCCLIENTCONTROLLER_H
#define RPCSERVER_RPCCLIENTCONTROLLER_H

#include "../manager/Closure.h"
#include <google/protobuf/service.h>


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
        failed = true;
        error_text = reason;
    }

    void StartCancel() override {
        canceled = true;
    }

    RpcClientController() : canceled(false), failed(false), error_text({}) {}
};

#endif //RPCSERVER_RPCCLIENTCONTROLLER_H
