//
// Created by 唐仁初 on 2022/8/17.
//

#include "RPCInterface.h"
#include "ServiceManager.h"

namespace tiny_swarm::manager {
    RPCInterface::RPCInterface(ServiceManager *manager) : manager_(manager) {

    }

    void RPCInterface::createService(RpcController *controller, const ServiceInfo *request, JsonMessage *response,
                                     Closure *done) {

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

    void RPCInterface::stopService(RpcController *controller, const ::ServiceName *request, ::ServiceName *response,
                                   Closure *done) {

        manager_->deleteService(request->token());

        response->set_token(request->token());
    }

    void RPCInterface::getAddressPool(RpcController *controller, const ::ServiceName *request, ::AddressPool *response,
                                      Closure *done) {

        auto json = manager_->getAccessAddress(request->token());

        if (json.empty()) {
            response->set_proxy_address("");
            return;
        }

        response->set_proxy_address(json["proxy_address"]);

        for (auto &addr: json["pods_address"])
            response->add_pod_address(addr);

    }

    void RPCInterface::getServiceInfo(RpcController *controller, const ::ServiceName *request, ::JsonMessage *response,
                                      Closure *done) {

        auto json = manager_->getServiceInformation(request->token());

        response->set_content(json.dump());
    }
}



