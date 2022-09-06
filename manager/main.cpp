//
// Created by 唐仁初 on 2022/7/27.
//

#include "Manager.h"
#include "WorkerClient.h"
#include "LogQueue.h"
#include "Result.h"
#include "RPCInterface.h"
#include "ServiceManager.h"
#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>



static void childSEFV(int signal){
    std::ofstream os("/Users/tangrenchu/CLionProjects/TinySwarm/logs.txt",std::ios::app);
    os << "in signal func" << std::endl;
    std::cout << signal << "called"<<std::endl;
    exit(139);
}

int LogQueueTest(){
    LogQueue logger("./log.txt");
    logger.start();
    logger.append("CW 127.0.0.1:8989");
    logger.append("DW 127.0.0.1:8989");
    logger.append("CS redis my_redis 0 nullptr nullptr 0");
    return 0;
}

std::map<int,std::string> map;

//
//auto WorkerRpcTest(){
//    WorkerClient client;
//    auto session = client.createSessionOnNewChannel(IPAddress(AF_INET, 8989));
//
//    ForkInput input;
//    input.set_service(std::string("hello"));
//    input.set_alias(std::string("my_hello"));
//    input.set_owner(std::string("127.0.0.1"));
//    input.set_type(host);
//    input.set_restart(0);
//
//    std::string error;
//    ForkEcho echo;
//    auto res = session->fork(&input,&echo,error);
//
//    std::cout << (echo.sd().alias())<< std::endl;
//
//    sleep(1);
//
//    CheckInput input1;
//    input1.set_user_info("127.0.0.1");
//    input1.set_line(10);
//    DownServices downService;
//
//    session->check(&input1,&downService,error);
//
//    for(int i =0;i<downService.service_size();i++){
//        const auto& err = downService.service(i);
//        std::cout << err.alias() << "\n"<<err.error_text() <<"\n"
//                  <<err.out_file()<<std::endl;
//    }
//
//    return res;
//}

void rpc_test(){
    Manager manager("127.0.0.1");

    manager.selectWorkerToCreatePod("redis","my_redis",docker,6379,
                                    {},{"-p","6379:6379"},0);

    auto services = manager.showServices("*");

    for(auto &r : services)
        std::cout << r->toSnapshotMessage() << std::endl;

    sleep(1);

    manager.connectToWorker(IPAddress("127.0.0.1",AF_INET,8989));

    auto wd = manager.findWorkerDescriptor("127.0.0.1",8989);

    auto res = manager.createService(wd,"redis","my_redis",docker,6379,
                                     {},{"-p","6379:6379"},0);

//    manager.pods.emplace("redis",PodDescriptor("redis","my_redis",docker,1,6379,wd,
//                                               {},{"-p","6379:6379"},0));

    if(res.isFail())
        std::cout << res.reason() << std::endl;

    services = manager.showServices("*");

    for(auto &r : services)
        std::cout << r->toSnapshotMessage() << std::endl;

    manager.snapshot();
//    auto info = manager.getServiceInfoIterByAlias("my_hello1");
//
//    manager.publishMessage(*info);

    ///for(;;);

}

void serviceJsonTest(){

    WorkerDescriptor wd;
    wd.ip = "127.0.0.1";

    PodDescriptor pd1("test", "test_1", docker, true, 8888, &wd,{"1","2"},{"-p","8888"}, 0);
    PodDescriptor pd2("test", "test_2", docker, true, 8888, &wd,{"1","2"},{"-p","8888"}, 0);

    manager::ServiceDescriptor sd("test",{&pd1,&pd2},2,"127.0.0.1",{});

    auto json = sd.toJson();

    auto string = nlohmann::json::to_bson(json);

    auto json2 = nlohmann::json::from_bson(string);

    std::cout << json2.dump(4);
}


int main(int argc ,char *argv[]){
    serviceJsonTest();

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