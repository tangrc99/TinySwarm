//
// Created by 唐仁初 on 2022/8/17.
//

#ifndef TINYSWARM_RPCINTERFACE_H
#define TINYSWARM_RPCINTERFACE_H

#include "manager/protos/ManagerRpcInterface.pb.h"


using google::protobuf::RpcController;
using google::protobuf::Closure;


namespace tiny_swarm::manager {

    class ServiceManager;

    /// Class RPCInterface describes manager's rpc service
    class RPCInterface final : public ManagerService {
    public:

        /// Construction
        /// \param manager manager to run this service
        explicit RPCInterface(ServiceManager *manager);

        /// Rpc Interface to create a service
        /// \param controller error controller
        /// \param request user's request
        /// \param response server's response
        /// \param done done class
        void createService(RpcController *controller, const ServiceInfo *request, JsonMessage *response,
                           Closure *done) override;
        /// Rpc Interface to stop a service
        /// \param controller error controller
        /// \param request user's request
        /// \param response server's response
        /// \param done done class
        void stopService(RpcController *controller, const ::ServiceName *request, ::ServiceName *response,
                         Closure *done) override;

        /// Rpc Interface to get address pool of a service
        /// \param controller error controller
        /// \param request user's request
        /// \param response server's response
        /// \param done done class
        void getAddressPool(RpcController *controller, const ::ServiceName *request, ::AddressPool *response,
                            Closure *done) override;

        /// Rpc Interface to get detailed information of a service
        /// \param controller error controller
        /// \param request user's request
        /// \param response server's response
        /// \param done done class
        void getServiceInfo(RpcController *controller, const ::ServiceName *request, ::JsonMessage *response,
                            Closure *done) override;


    private:
        ServiceManager *manager_;
    };

}


#endif //TINYSWARM_RPCINTERFACE_H
