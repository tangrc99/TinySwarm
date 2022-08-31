//
// Created by 唐仁初 on 2022/8/29.
//

#include "IPAddress.h"
#include "RpcInterface.h"
#include "Common/RPCServer.h"

int main() {

    auto log_controller = LoggerController::getInstance();
    log_controller.init("../../properties/log4cxx.properties", "fileappender");


    RPCServer server(IPAddress(AF_INET, 8989));

    RpcInterface interface;

    server.addService(&interface);

    server.startService();

    return 0;
}