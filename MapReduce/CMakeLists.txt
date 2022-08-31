cmake_minimum_required(VERSION 3.16)
project(MapReduce)

set(CMAKE_CXX_STANDARD 17)


add_executable(MapReduce
        main.cpp
        MapReduce.cpp
        MapReduce.h
        Mongo.h
        Mongo.cpp
        MongoImg.h
        MongoImg.cpp

        ${COMMON_SRCS}

        #Common/HTTPData.h Common/HTTPData.cpp


        YoloInfer.pb.h YoloInfer.pb.cc
        #Common/RpcResult.h
        MapReduceTask.cpp MapReduceTask.h AddressPool.cpp AddressPool.h ManagerClient.cpp ManagerClient.h)


target_link_libraries(MapReduce mongocxx protobuf)