cmake_minimum_required(VERSION 3.16)
project(TCPServer)

set(CMAKE_CXX_STANDARD 17)

set(TCP_SRC  Poller.cpp
        Poller.h
        Buffer.h
        Buffer.cpp
        ThreadPool.h
        ThreadPool.cpp
        Channel.h
        Channel.cpp
        Acceptor.h
        Acceptor.cpp
        EventLoop.h
        EventLoop.cpp
        TCPConnection.h
        TCPConnection.cpp
        TCPServer.cpp
        TCPServer.h
        Timer.cpp
        Timer.h)

add_library(TCPServer ${TCP_SRC})

target_link_libraries(TCPServer log4cxx)