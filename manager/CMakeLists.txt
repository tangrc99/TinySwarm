cmake_minimum_required(VERSION 3.20)
project(manager)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR})

aux_source_directory(Proxy PROXY_SRCS)

find_package(nlohmann_json 3.2.0 REQUIRED)


add_executable(manager
        main.cpp
        Manager.cpp Manager.h
        WorkerDescriptor.h

        WorkerClient.h

        protos/WorkerRpcInterface.pb.cc
        protos/WorkerRpcInterface.pb.h


        ${COMMON_SRCS}
        ${PROXY_SRCS}


        ../LogUtils/LoggerControl.cpp
        ../LogUtils/LoggerControl.h
        ../LogUtils/Logging.h

        protos/ManagerRpcInterface.pb.cc
        protos/ManagerRpcInterface.pb.h


        PodDescriptor.h
        LogQueue.cpp LogQueue.h
        RPCInterface.cpp RPCInterface.h
        ServiceManager.h
        ServiceDescriptor.h Scheduler/Scheduler.h Result.h PodDescriptor.cpp ServiceManager.cpp
        )


target_link_libraries(manager protobuf libTCPServer.a log4cxx hiredis redis++ nlohmann_json::nlohmann_json)