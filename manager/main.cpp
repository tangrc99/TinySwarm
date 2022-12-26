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


static void childSEFV(int signal) {
    std::ofstream os("/Users/tangrenchu/CLionProjects/TinySwarm/logs.txt", std::ios::app);
    os << "in signal func" << std::endl;
    std::cout << signal << "called" << std::endl;
    exit(139);
}

using namespace manager;

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

//    auto info = manager.getServiceInfoIterByAlias("my_hello1");
//
//    manager.publishMessage(*info);

    ///for(;;);

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


static int exit_flag;

static void exit_func(int) {
    std::cout << "received signal" << std::endl;
    exit_flag = 1;
}

int signal_exit_test() {

    auto addr = IPAddress::getIpByHost("www.baidu.com");

    IPAddress ip(addr[0] + ":80");


    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    int fd2 = ::socket(AF_INET, SOCK_STREAM, 0);

    auto res = ::connect(fd, ip.toSockAddrPtr(), ip.addrlen());
    auto res2 = ::connect(fd2, ip.toSockAddrPtr(), ip.addrlen());

    char buff[100];

    std::thread th1([&buff, &fd] {

        struct sigaction act{};
        act.sa_flags = 0;
        act.sa_handler = emptyFunction;

        sigaction(SIGALRM,&act, nullptr);
        std::cout << "wait for response " << std::endl;
        ::read(fd, buff, 100);
        std::cout << "end " << errno << std::endl;

    });

    std::thread th2([&buff, &fd2] {

        struct sigaction act{};
        act.sa_flags = 0;
        act.sa_handler = emptyFunction;

        sigaction(SIGALRM,&act, nullptr);
        std::cout << "wait for response " << std::endl;
        ::read(fd2, buff, 100);
        std::cout << "end " << std::this_thread::get_id() << std::endl;

    });

    sleep(3);

    pthread_kill(th1.native_handle(), SIGALRM);
   // pthread_kill(th2.native_handle(), SIGALRM);

    th1.join();
    th2.join();

    return 0;
}


int main(int argc, char *argv[]) {


    return signal_exit_test();
    auto log_controller = LoggerController::getInstance();
    log_controller.init("../../properties/manager.properties", "fileappender");

    manager::ServiceManager manager(IPAddress(AF_INET, 8888));

    manager.connectToWorker(IPAddress("127.0.0.1", AF_INET, 8989));

    //auto create_res = manager.createService("my_redis", 1, "redis", docker, 6379, {}, {"-p", "6379:6379"});

    manager.run();

    // std::cout << create_res.dump(4);

    auto workers = manager.showWorkerNodes();


    std::cout << workers.dump(4);

    //   std::cout << "json_out: " << create_res.dump(4);

    return 0;

//
//    sleep(5);
//
//    manager.checkServicesOnAllWorker(10);
//
//    for(auto & service : manager.down_services){
//        std::cout << service->alias_ << "\n"
//        << service->error->reason << "\n"
////        << service->error->exit <<std::endl;
////    }
//
//    auto wd = manager.connectToWorker(IPAddress("127.0.0.1",AF_INET, 8989));
////
////
//    //auto wd = manager.findWorkerDescriptor("127.0.0.1",8989);
//
//    auto [res,error] = manager.createService(wd,"redis","my_redis",docker,
//                                             {},{"-p","6379:6379"},0);
//
//
////
////    manager.snapshot();
//    return 0;
}