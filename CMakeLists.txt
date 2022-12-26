cmake_minimum_required(VERSION 3.16)
project(TinySwarm)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory( ${CMAKE_SOURCE_DIR}/Common COMMON_SRCS)
aux_source_directory( ${CMAKE_SOURCE_DIR}/LogUtils LOG_SRCS)


include_directories(${CMAKE_SOURCE_DIR})

include_directories(LogUtils)
include_directories(TCPServer)
include_directories(Common)
link_directories(lib)

include_directories("/usr/local/include")
link_directories("/usr/local/lib")


add_subdirectory(worker)
add_subdirectory(manager)

