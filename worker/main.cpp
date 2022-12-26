//
// Created by 唐仁初 on 2022/7/27.
//

#include "WorkerNode.h"

using namespace tiny_swarm::worker;

void rpc_test() {

    auto log_controller = LoggerController::getInstance();
    log_controller.init("../../properties/log4cxx.properties", "fileappender");

    WorkerNode node("/Users/tangrenchu/CLionProjects/TinySwarm/Services");


    node.start();

//    node.server = new RPCServer(addr,functors);
//
//    RPCInterface service(&node);
//    node.server->addService(&service);
//
//    node.server->startService();
//
//    for(auto &service : node.services){
//        std::cout <<  service.pid << std::endl;
//        node.stopService(service);
//    }
}


int main() {

    rpc_test();
    return 0;
//    std::vector<char *> params = {"/usr/local/bin/docker","run","--name","sh_redis","redis",nullptr};
//    auto res = execv("/usr/local/bin/docker", &params[0]);
//
//    return res;
    WorkerNode node("/Users/tangrenchu/CLionProjects/TinySwarm/Services");
    //RPCInterface rpc(&node);


    std::string error;
    auto pid = node.createProcess
            ("hello", "123", "0", 0, -1, {}, {}, error, 0);

//
    //std::cout << "to kill" << pid <<std::endl;
    sleep(5);
    node.checkIfPodsRunning();



    return 0;

}