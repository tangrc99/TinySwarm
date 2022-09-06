cmake_minimum_required(VERSION 3.20)
project(manager)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR})

aux_source_directory(Proxy PROXY_SRCS)

add_executable(manager main.cpp Manager.cpp Manager.h WorkerDescriptor.h

        WorkerClient.h

        WorkerRpcInterface.pb.cc
        WorkerRpcInterface.pb.h


        ${COMMON_SRCS}
        ${PROXY_SRCS}


        ../LogUtils/LoggerControl.cpp
        ../LogUtils/LoggerControl.h
        ../LogUtils/Logging.h

        ManagerRpcInterface.pb.cc ManagerRpcInterface.pb.h

        #RpcSession.cpp RpcSession.h
        PodDescriptor.h
        LogQueue.cpp LogQueue.h
        RPCInterface.cpp RPCInterface.h
        ServiceManager.h
        ManagerError.h ServiceDescriptor.h Scheduler/Scheduler.h Result.h PodDescriptor.cpp ServiceManager.cpp Proxy/NoProxy.h)


target_link_libraries(manager protobuf libTCPServer.a log4cxx hiredis redis++)