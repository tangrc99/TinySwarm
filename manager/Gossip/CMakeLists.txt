cmake_minimum_required(VERSION 3.20)
project(Gossip)

set(CMAKE_CXX_STANDARD 20)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(logUtils)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories("/usr/local/include")
link_directories("/usr/local/lib")

add_executable(Gossip src/main.cpp
        src/GossipNode.cpp
        src/GossipNode.h src/GossipValueTable.cpp src/GossipValueTable.h src/IOHandler.cpp src/IOHandler.h Common/CommonStruct.h src/Poller.cpp src/Poller.h
        Common/HTTPData.cpp
        Common/HTTPData.h
        Common/HTTPParser.h
        Common/HTTPParser.cpp
        logUtils/LoggerControl.cpp
        logUtils/LoggerControl.h
        logUtils/Logging.h
        src/RedisGossipTable.cpp src/RedisGossipTable.h Common/CommonParams.h src/UserInterface.h src/UserCommandTable.cpp src/UserCommandTable.h)

add_executable(usr src/UserInterface.cpp src/UserInterface.h)

target_link_libraries(Gossip log4cxx hiredis redis++)

