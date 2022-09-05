//
// Created by 唐仁初 on 2022/7/28.
//

#ifndef TINYSWARM_RPCINTERFACE_H
#define TINYSWARM_RPCINTERFACE_H


#include "WorkerRpcInterface.pb.h"
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace worker {

    class WorkerNode;

    class RPCInterface final : public WorkerService {

    public:

        explicit RPCInterface(WorkerNode *node) {
            owner = node;
        }

        void fork(::google::protobuf::RpcController *controller, const ::ForkInput *request, ::ForkEcho *response,
                  ::google::protobuf::Closure *done) override;

        void
        shutdown(::google::protobuf::RpcController *controller, const ::ShutdownInput *request, ::Echo *response,
                 ::google::protobuf::Closure *done) override;

        void check(::google::protobuf::RpcController *controller, const ::CheckInput *request, ::DownServices *response,
                   ::google::protobuf::Closure *done) override;


    private:

        WorkerNode *owner;
    };
}

#endif //TINYSWARM_RPCINTERFACE_H
