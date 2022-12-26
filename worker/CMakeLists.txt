cmake_minimum_required(VERSION 3.20)
project(worker)

set(CMAKE_CXX_STANDARD 17)

find_package(nlohmann_json 3.2.0 REQUIRED)


add_executable(worker main.cpp WorkerNode.cpp WorkerNode.h PodDescriptor.h RPCInterface.cpp RPCInterface.h
        ../Common/MsgPack.pb.cc ../Common/MsgPack.pb.h protos/WorkerRpcInterface.pb.cc protos/WorkerRpcInterface.pb.h

        ../LogUtils/LoggerControl.cpp
        ../LogUtils/LoggerControl.h
        ../LogUtils/Logging.h
        ../Common/RPCServer.h
        ../Common/RPCServer.cpp
        ../Common/HTTPData.cpp
        ../Common/HTTPData.h
        WorkerStatus.cpp WorkerStatus.h
        ManagerDescriptor.h
        recorder/PodStartRecorder.cpp recorder/PodStartRecorder.h
        WorkerError.h
        register/Register.cpp register/Register.h
        probe/LiveProbe.h
        PodDescriptor.cpp

        ErrorReporter.cpp ErrorReporter.h
        util/LockFreeQueue.h
        Mongo.h)



target_link_libraries(worker protobuf protobuf-lite libTCPServer.a log4cxx mongocxx bsconcxx nlohmann_json::nlohmann_json)