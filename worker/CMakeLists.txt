cmake_minimum_required(VERSION 3.20)
project(worker)

set(CMAKE_CXX_STANDARD 17)

add_executable(worker main.cpp WorkerNode.cpp WorkerNode.h PodDescriptor.h RPCInterface.cpp RPCInterface.h
        ../Common/MsgPack.pb.cc ../Common/MsgPack.pb.h WorkerRpcInterface.pb.cc WorkerRpcInterface.pb.h

        ../LogUtils/LoggerControl.cpp
        ../LogUtils/LoggerControl.h
        ../LogUtils/Logging.h
        ../Common/RPCServer.h
        ../Common/RPCServer.cpp
        ../Common/HTTPData.cpp
        ../Common/HTTPData.h WorkerStatus.cpp WorkerStatus.h ManagerDescriptor.h PodRecorder.cpp PodRecorder.h WorkerError.h Register.cpp Register.h)


target_link_libraries(worker protobuf libTCPServer.a log4cxx)