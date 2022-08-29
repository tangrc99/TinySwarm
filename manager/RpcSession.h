//
// Created by 唐仁初 on 2022/8/2.
//

#ifndef TINYSWARM_RPCSESSION_H
#define TINYSWARM_RPCSESSION_H

#include "IPAddress.h"
#include "Session.h"
#include "WorkerRpcInterface.pb.h"
#include "RpcClientChannel.h"
#include "RpcClientController.h"
#include <memory>


class RpcSession : public Session {
public:

    explicit RpcSession(const std::shared_ptr<RpcClientChannel> &channel) : Session(channel) {
        stub = std::make_shared<WorkerService_Stub>(channel_.get());
    }

    // 这里的逻辑是分层的，返回值返回的是 rpc 通信层次的错误，服务创建的错误在echo包里面
    bool fork(const ForkInput *request, ForkEcho *echo, std::string &error_text) {

        controller_.Reset();
        stub->fork(&controller_, request, echo, {});

        timestamp = time(nullptr);

        if (controller_.Failed()) {
            error_text = controller_.ErrorText();
            return false;
        }
        return true;
    }

    bool shutdown(const ShutdownInput *request, Echo *echo, std::string &error_text) {
        controller_.Reset();
        stub->shutdown(&controller_, request, echo, {});

        timestamp = time(nullptr);

        if (controller_.Failed()) {
            error_text = controller_.ErrorText();
            return false;
        }
        return true;
    }

    bool check(const CheckInput *request, DownServices *echo, std::string &error_text) {
        controller_.Reset();
        stub->check(&controller_, request, echo, {});

        timestamp = time(nullptr);

        if (controller_.Failed()) {
            error_text = controller_.ErrorText();
            return false;
        }
        return true;
    }



private:
    std::shared_ptr<WorkerService_Stub> stub;

};

#endif //TINYSWARM_RPCSESSION_H
