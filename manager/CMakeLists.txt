cmake_minimum_required(VERSION 3.20)
project(manager)

set(CMAKE_CXX_STANDARD 17)

add_executable(manager main.cpp Manager.cpp Manager.h WorkerDescriptor.h
        ../Common/RpcClientChannel.h
        ../Common/RpcClientController.h
        Closure.h
        WorkerClient.h
        WorkerRpcInterface.pb.cc
        WorkerRpcInterface.pb.h
        ../Common/MsgPack.pb.h
        ../Common/MsgPack.pb.cc

        ../Common/RPCServer.cpp ../Common/RPCServer.h

        ../logUtils/LoggerControl.cpp
        ../logUtils/LoggerControl.h
        ../logUtils/Logging.h

        ../Common/IPAddress.h
        ../Common/IPAddress.cpp
        ../Common/HTTPData.cpp
        ../Common/HTTPData.h RpcSession.cpp RpcSession.h ServiceImplInfo.h
        LogQueue.cpp LogQueue.h
        RPCInterface.cpp RPCInterface.h MRManager.cpp MRManager.h)


target_link_libraries(manager protobuf libTCPServer.a log4cxx hiredis redis++)