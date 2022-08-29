


#include "UserInterface.h"
#include "RedisGossipTable.h"
#include <iostream>
#include <float.h>

void print(int a ){
    std::cout<<a<<"\n";
}

int main() {

    sw::redis::Redis redis("tcp://127.0.0.1:6379");
    redis.zadd("Servers","6.6.6.6",9999999999);

    auto log_controller = LoggerController::getInstance();
    log_controller.init("../properties/log4cxx1.properties", "fileappender");



    GossipNode gossip(IPAddress(AF_INET,7000),"../properties/GossipNodes",1);

    gossip.EventLoop();


//    UserInterface ui;
//    ui.run();

    LoggerController::exit();



    return 0;
}
