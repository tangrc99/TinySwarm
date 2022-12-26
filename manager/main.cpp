//
// Created by 唐仁初 on 2022/7/27.
//

#include "Manager.h"
#include "WorkerClient.h"
#include "LogQueue.h"
#include "Result.h"
#include "RPCInterface.h"
#include "ServiceManager.h"
#include "Proxy/NginxProxy.h"

#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>



using namespace tiny_swarm;
using namespace tiny_swarm::manager;

void rpc_test() {
    Manager manager("127.0.0.1");

    manager.selectWorkerToCreatePod("redis", "my_redis", docker, 6379,
                                    {}, {"-p", "6379:6379"}, 0);

    auto services = manager.showServices("*");

    for (auto &r: services)
        std::cout << r->toSnapshotMessage() << std::endl;

    sleep(1);

    manager.connectToWorker(IPAddress("127.0.0.1", AF_INET, 8989));

    auto wd = manager.findWorkerDescriptor("127.0.0.1", 8989);

    auto res = manager.createPod(wd, "redis", "my_redis", docker, 6379,
                                 {}, {"-p", "6379:6379"}, 0);

//    manager.pods.emplace("redis",PodDescriptor("redis","my_redis",docker,1,6379,wd,
//                                               {},{"-p","6379:6379"},0));

    if (res.isFail())
        std::cout << res.reason() << std::endl;

    services = manager.showServices("*");

    for (auto &r: services)
        std::cout << r->toSnapshotMessage() << std::endl;


}

void serviceJsonTest() {

    WorkerDescriptor wd;
    wd.ip = "127.0.0.1";

    PodDescriptor pd1("test", "test_1", docker, true, 8888, &wd, {"1", "2"}, {"-p", "8888"}, 0);
    PodDescriptor pd2("test", "test_2", docker, true, 8888, &wd, {"1", "2"}, {"-p", "8888"}, 0);

    manager::ServiceDescriptor sd("test", {&pd1, &pd2}, 2, "127.0.0.1", {});

    auto json = sd.toJson();

    auto string = nlohmann::json::to_bson(json);

    auto json2 = nlohmann::json::from_bson(string);

    std::cout << json2.dump(4);
}




int main(int argc, char *argv[]) {


    auto log_controller = LoggerController::getInstance();
    log_controller.init("../../properties/manager.properties", "fileappender");

    manager::ServiceManager manager(IPAddress(AF_INET, 8888));

    manager.connectToWorker(IPAddress("127.0.0.1", AF_INET, 8989));


    manager.run();



    return 0;


}