cmake_minimum_required(VERSION 3.16)
project(yolo)

set(CMAKE_CXX_STANDARD 17)

add_executable(yolo main.cpp main.cpp RpcInterface.cpp RpcInterface.h ${COMMON_SRCS} ${LOG_SRCS}
        YoloInfer.pb.cc
        YoloInfer.pb.h)


target_link_libraries(yolo protobuf libTCPServer.a log4cxx)