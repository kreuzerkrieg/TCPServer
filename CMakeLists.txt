cmake_minimum_required(VERSION 3.10)
project(TCPServer)

set(CMAKE_CXX_STANDARD 14)

add_executable(TCPServer main.cpp)

#FIND_LIBRARY(SYSLIBS NAMES
#        fbzmq
#        folly
#        zmq
#        sodium
#        )

find_library(FBZMQ fbzmq)
#find_library(DOUBLECONV double-conversion)
find_library(GFLAGS gflags)
find_library(GLOG glog)
find_library(PTHREAD pthread)
find_library(SODIUM sodium)
find_library(ZMQ zmq)

find_library(FOLLY folly PATHS)
#find_library(ZSTD zstd)

#find_library(THRIFTCPP2 thriftcpp2)
#find_library(THRIFTPROTOCOL thriftprotocol)
#find_library(PROTOCOL protocol)
#find_library(TRANSPORT transport)

TARGET_LINK_LIBRARIES(TCPServer
        ${FBZMQ}
        #        ${ZSTD}
                ${FOLLY}
        #        ${DOUBLECONV}
                ${GFLAGS}
                ${GLOG}
                ${ZMQ}
                ${SODIUM}
                ${PTHREAD}
        #        ${THRIFTPROTOCOL}
        #        ${PROTOCOL}
        #        ${TRANSPORT}

        )
#include_directories(../fbzmq/)