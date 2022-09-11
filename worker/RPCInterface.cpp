//
// Created by 唐仁初 on 2022/7/28.
//

#include "RPCInterface.h"
#include "WorkerNode.h"

namespace worker {

    void
    RPCInterface::fork(::google::protobuf::RpcController *controller, const ::ForkInput *request, ::ForkEcho *response,
                       ::google::protobuf::Closure *done) {


        std::string error_text;

        // 如果当前 worker 节点不提供该服务
        if (!owner->isServiceExist(request->service())) {
            auto sd = response->sd().New();
            sd->set_service(request->service().c_str());
            sd->set_alias(request->alias().c_str());
            sd->set_type(request->type());
            response->set_fail(true);
            response->set_error_text(SERV_NOT_FOUND);
            return;
        }

        // 检查服务是否是重复创建的，以及alias是否可用
        if(owner->checkIfAliasOK(request->owner(),request->alias(),error_text)){
            auto sd = response->sd().New();
            sd->set_service(request->service().c_str());
            sd->set_alias(request->alias().c_str());
            sd->set_type(request->type());
            response->set_fail(true);
            response->set_error_text(error_text);
            return;
        }

        // 判断端口是否可用
        {
            int port = request->port();
            int fd = ::socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
            IPAddress address(AF_INET,port);
            int res = ::bind(fd,address.toSockAddrPtr(),address.addrlen());
            ::close(fd);
            if(res < 0){
                response->set_fail(true);
                response->set_error_text("Port Unavailable");
                auto sd = response->sd().New();
                sd->set_service(request->service().c_str());
                sd->set_alias(request->alias().c_str());
                sd->set_type(request->type());
                return ;
            }
        }

        //FIXME : 这里要判断之前是否已经有过记录的文件

        std::cout << request->owner();

        // 如果该节点之前被移除, 要求对侧节点必须移除所有信息，然后再次重试
        if (owner->isManagerDown(request->owner())) {

            auto sd = response->sd().New();
            sd->set_service(request->service().c_str());
            sd->set_alias(request->alias().c_str());
            sd->set_type(request->type());
            response->set_fail(true);
            response->set_error_text("Manager Down");
            return;
        }

        std::cout << " calling fork" << std::endl;



        // 进行命令的格式转换
        std::vector<char *> exe_params;
        exe_params.reserve(request->exe_params_size());
        for (int i = 0; i < request->exe_params_size(); i++) {
            exe_params.emplace_back(const_cast<char *>(request->exe_params(i).c_str()));
        }

        std::vector<char *> docker_params;
        docker_params.reserve(request->docker_params_size());
        for (int i = 0; i < request->docker_params_size(); i++) {
            docker_params.emplace_back(const_cast<char *>(request->docker_params(i).c_str()));
        }


        std::cout << request->service() << " " << request->alias() << std::endl;

        auto pid = owner->createProcess(request->service(), request->alias(), request->owner(),
                                        request->type() == host ? 0 : 1, request->port(), exe_params,
                                        docker_params, error_text, request->restart());

        std::cout << "Fork " << pid << std::endl;

        if (pid > 0) {
            auto sd = response->sd().New();
            sd->set_service(request->service().c_str());
            sd->set_alias(request->alias().c_str());
            sd->set_type(request->type());

            response->set_allocated_sd(sd);
            response->set_fail(false);
            response->set_error_text(error_text);

            // 存储进程的启动信息
            owner->addPodRecord(request);

        } else {
            auto sd = response->sd().New();
            sd->set_service(request->service().c_str());
            sd->set_alias(request->alias().c_str());
            sd->set_type(request->type());
            response->set_fail(true);
            response->set_error_text(error_text);
        }

        // FIXME :没写错误处理

        std::cout << "fork end" << std::endl;

    }

    void
    RPCInterface::shutdown(::google::protobuf::RpcController *controller, const ::ShutdownInput *request,
                           ::Echo *response,
                           ::google::protobuf::Closure *done) {

        // 如果该节点之前被移除, 要求对侧节点必须移除所有信息，然后再次重试
        if (owner->isManagerDown(request->user_info())) {

            response->set_fail(true);
            response->set_error_text("Manager Down");
            return;
        }

        int res;
        if (request->sd().type() == ServiceType::host) {
            res = owner->stopPod(request->sd().alias(), request->user_info());
        } else {
            res = owner->stopDockerPod(request->sd().alias(), request->user_info());
        }

        response->set_fail(res != 0);

        switch (res) {
            case -2:
                response->set_error_text(ALIAS_NOT_FOUND);
                break;
            case -1:
                response->set_error_text(KILL_ERROR);
                break;
            case 0:
                response->set_error_text(SUCCESS);
                break;
            case 1:
                response->set_error_text(USR_ERROR);
                break;
            default:
                break;
        }
    }


    void RPCInterface::check(::google::protobuf::RpcController *controller, const ::CheckInput *request,
                             ::DownServices *response, ::google::protobuf::Closure *done) {

        std::cout << "check start " << std::endl;
        std::list<PodExitInformation> exits;

        // FIXME: 如果收到的 manager 不在当前的列表里面，那么就代表当前的 manager 已经被移出掉了；
        // 但是如果 manager 断开后，再次收到了 create 请求，怎么办？
        if (owner->isManagerDown(request->user_info())) {

            DownService *error = response->add_service();
            error->set_alias("*");
            error->set_error_text("Manager Down");
            error->set_exit_num(0);
            error->set_out_file("Manager Down");

            return;
        }


        // 收集错误信息
        owner->collectExitInformation(request->user_info(), exits);

        for (auto &it: exits) {
            DownService *error = response->add_service();
            error->set_alias(it.alias);
            error->set_error_text(it.getErrorText());

            error->set_exit_num(it.exit_num);
            error->set_out_file(it.getOutFile(request->line()));
        }

        owner->cleanDumpFiles(exits);

    }
}
