//
// Created by 唐仁初 on 2022/8/17.
//

#ifndef TINYSWARM_RPCINTERFACE_H
#define TINYSWARM_RPCINTERFACE_H

#include "Manager.h"
#include "Proxy/NginxProxy.h"
#include "ServiceManager.h"
#include "ManagerRpcInterface.pb.h"
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>


using google::protobuf::RpcController;
using google::protobuf::Closure;


namespace manager {

    class RPCInterface final : public ManagerService {
    public:

        explicit RPCInterface(Manager *manager) : manager_(manager),
                                                  g_manager_(new ServiceManager(manager_, new NginxProxy("path"))) {

        }


        void createService(RpcController *controller, const ServiceInformation *request, CreateOrDeleteEcho *response,
                           Closure *done) override {

          ///  auto worker = manager_->selectWorker();

            std::vector<char *> exe_params(request->exe_params_size());
            for (int i = 0; i < request->exe_params_size(); i++) {
                exe_params.emplace_back(const_cast<char *>(request->exe_params(i).c_str()));
            }

            std::vector<char *> docker_params(request->docker_params_size());
            for (int i = 0; i < request->docker_params_size(); i++) {
                docker_params.emplace_back(const_cast<char *>(request->docker_params(i).c_str()));
            }

//            auto[res, error] = manager_->createService(worker, request->service(), request->alias(),
//                                                       request->type() == 0 ? host : docker, request->port(),
//                                                       exe_params,
//                                                       docker_params, request->restart());

//            response->set_fail(res);
//            response->set_info(error);
        }

        void
        showService(RpcController *controller, const ::ServiceName *request, ::ServiceList *response,
                    Closure *done) override {

            // 显示全部的服务
            if (request->name_or_alias() == "*") {
                auto services = manager_->getServices();

                for (auto &pair: *services) {
                    auto serv = response->add_services();
                    serv->set_service(pair.second.service_);
                    serv->set_alias(pair.second.alias_);
                    serv->set_type(pair.second.type_ == host ? 0 : 1);

                    for (auto &param: pair.second.exe_params_) {
                        std::string *exe_param = serv->add_exe_params();
                        *exe_param = param;
                    }
                    for (auto &param: pair.second.docker_params_) {
                        std::string *docker_param = serv->add_docker_params();
                        *docker_param = param;
                    }
                    serv->set_restart(pair.second.restart_);
                }

                return;
            }

            auto service = manager_->getServiceInfoIterByAlias(request->name_or_alias());
            auto serv = response->add_services();
            serv->set_service(service->service_);
            serv->set_alias(service->alias_);
            serv->set_type(service->type_ == host ? 0 : 1);

            for (auto &param: service->exe_params_) {
                std::string *exe_param = serv->add_exe_params();
                *exe_param = param;
            }
            for (auto &param: service->docker_params_) {
                std::string *docker_param = serv->add_docker_params();
                *docker_param = param;
            }
            serv->set_restart(service->restart_);

        }

        void
        stopService(RpcController *controller, const ::ServiceName *request, ::CreateOrDeleteEcho *response,
                    Closure *done) override {

            auto service = manager_->getServiceInfoIterByAlias(request->name_or_alias());

            auto[res, error] = manager_->stopService(*service);

            response->set_fail(res);
            response->set_info(error);
        }

        void transferService(RpcController *controller, const ::TransferInfo *request,
                             ::CreateOrDeleteEcho *response, Closure *done) override {

            auto service = manager_->getServiceInfoIterByAlias(request->alias());
            auto wd = manager_->findWorkerDescriptor(request->address());
            auto[res, error] =manager_->transferService(*service, wd);

            response->set_fail(res);
            response->set_info(error);
        }

        void
        createServiceGroup(RpcController *controller, const ServiceGroupName *request, CreateOrDeleteEcho *response,
                           Closure *done) override {

            std::vector<char *> exe_params(request->exe_params_size());
            for (int i = 0; i < request->exe_params_size(); i++) {
                exe_params.emplace_back(const_cast<char *>(request->exe_params(i).c_str()));
            }

            std::vector<char *> docker_params(request->docker_params_size());
            for (int i = 0; i < request->docker_params_size(); i++) {
                docker_params.emplace_back(const_cast<char *>(request->docker_params(i).c_str()));
            }

//            g_manager_->createServiceGroup(request->token(), request->service_num(), request->service(),
//                                           request->type() == 0 ? host : docker, request.,exe_params, docker_params,
//                                           request->restart());

//        response->set_fail();
//        response->set_info();
        }

        void getServiceGroupInfo(RpcController *controller, const ::ServiceGroup *request,
                                 ::ServiceGroup *response, Closure *done) override {

        }

        void getAddressPool(RpcController *controller, const ::ServiceGroup *request,
                            ::AddressPool *response, Closure *done) override {}

        void stopServiceGroup(RpcController *controller, const ::ServiceGroup *request,
                              ::CreateOrDeleteEcho *response, Closure *done) override {}


    private:
        Manager *manager_;
        ServiceManager *g_manager_;
    };

}


#endif //TINYSWARM_RPCINTERFACE_H
