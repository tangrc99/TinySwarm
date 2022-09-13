//
// Created by 唐仁初 on 2022/8/17.
//

#ifndef TINYSWARM_RPCINTERFACE_H
#define TINYSWARM_RPCINTERFACE_H

#include "ManagerRpcInterface.pb.h"


using google::protobuf::RpcController;
using google::protobuf::Closure;


namespace manager {

    class ServiceManager;

    class RPCInterface final : public ManagerService {
    public:

        explicit RPCInterface(ServiceManager *manager);

        void createService(RpcController *controller, const ServiceInfo *request, JsonMessage *response,
                           Closure *done) override;

        void stopService(RpcController *controller, const ::ServiceName *request, ::ServiceName *response,
                         Closure *done) override;


        void getAddressPool(RpcController *controller, const ::ServiceName *request, ::AddressPool *response,
                            Closure *done) override;

        void getServiceInfo(RpcController *controller, const ::ServiceName *request, ::JsonMessage *response,
                            Closure *done) override;


    private:
        ServiceManager *manager_;
    };

}


#endif //TINYSWARM_RPCINTERFACE_H
