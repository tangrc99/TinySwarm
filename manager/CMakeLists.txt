cmake_minimum_required(VERSION 3.20)
project(manager)

set(CMAKE_CXX_STANDARD 17)

add_executable(manager main.cpp Manager.cpp Manager.h WorkerDescriptor.h

        WorkerClient.h
        WorkerRpcInterface.pb.cc
        WorkerRpcInterface.pb.h


        ${COMMON_SRCS}


        ../LogUtils/LoggerControl.cpp
        ../LogUtils/LoggerControl.h
        ../LogUtils/Logging.h


        #RpcSession.cpp RpcSession.h
        ServiceImplInfo.h
        LogQueue.cpp LogQueue.h
        RPCInterface.cpp RPCInterface.h MRManager.cpp GroupManager.h)


target_link_libraries(manager protobuf libTCPServer.a log4cxx hiredis redis++)