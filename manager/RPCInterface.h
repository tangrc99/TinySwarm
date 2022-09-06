//
// Created by 唐仁初 on 2022/8/17.
//

#ifndef TINYSWARM_RPCINTERFACE_H
#define TINYSWARM_RPCINTERFACE_H

#include "Manager.h"
#include "ServiceManager.h"
#include "ManagerRpcInterface.pb.h"
//#include <google/protobuf/service.h>
//#include <google/protobuf/descriptor.h>
//#include <google/protobuf/message.h>


using google::protobuf::RpcController;
using google::protobuf::Closure;


namespace manager {

    class RPCInterface final : public ManagerService {
    public:

        explicit RPCInterface(Manager *manager) : manager_(new ServiceManager(manager, {})) {

        }


        void createService(RpcController *controller, const ServiceInfo *request, JsonMessage *response,
                           Closure *done) override {

            const auto &params = request->params();

            std::vector<char *> exe_params(params.exe_params_size());
            for (int i = 0; i < params.exe_params_size(); i++) {
                exe_params.emplace_back(const_cast<char *>(params.exe_params(i).c_str()));
            }

            std::vector<char *> docker_params(params.docker_params_size());
            for (int i = 0; i < params.docker_params_size(); i++) {
                docker_params.emplace_back(const_cast<char *>(params.docker_params(i).c_str()));
            }

            auto json = manager_->createService(request->name().token(), params.service_num(), params.service(),
                                                params.type() == 0 ? host : docker, params.port(), exe_params,
                                                docker_params,
                                                params.restart());

            response->set_content(json.dump());
        }

        void stopService(RpcController *controller, const ::ServiceName *request, ::ServiceName *response,
                         Closure *done) override {

            manager_->deleteService(request->token());

            response->set_token(request->token());
        }


        void getAddressPool(RpcController *controller, const ::ServiceName *request, ::AddressPool *response,
                            Closure *done) override {

            auto json = manager_->getAccessAddress(request->token());

            if (json.empty()) {
                response->set_proxy_address("");
                return;
            }

            response->set_proxy_address(json["proxy_address"]);

            for (auto &addr: json["pods_address"])
                response->add_pod_address(addr);

        }

        void getServiceInfo(RpcController *controller, const ::ServiceName *request, ::JsonMessage *response,
                            Closure *done) override {

            auto json = manager_->getServiceInformation(request->token());

            response->set_content(json.dump());
        }


    private:
        ServiceManager *manager_;
    };

}


#endif //TINYSWARM_RPCINTERFACE_H
