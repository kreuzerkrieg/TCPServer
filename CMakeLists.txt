cmake_minimum_required(VERSION 3.10)
project(MQServer/Client)

set(CMAKE_CXX_STANDARD 14)

add_executable(Client ZServer.h ZClient.cpp ZClient.h ZConfig.cpp ZConfig.h MQServerRunner.h client.cpp)
add_executable(Server ZServer.h ZClient.cpp ZClient.h ZConfig.cpp ZConfig.h MQServerRunner.h server.cpp)

find_library(FBZMQ fbzmq)
find_library(DOUBLECONV double-conversion)
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

SET_TARGET_PROPERTIES(Client
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
        )

TARGET_LINK_LIBRARIES(Client
        ${FBZMQ}
        #        ${ZSTD}
        ${FOLLY}
        ${DOUBLECONV}
        ${GFLAGS}
        ${GLOG}
        ${ZMQ}
        ${SODIUM}
        ${PTHREAD}
        #${THRIFTPROTOCOL}
        #${PROTOCOL}
        #${TRANSPORT}
        )

SET_TARGET_PROPERTIES(Server
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
        )

TARGET_LINK_LIBRARIES(Server
        ${FBZMQ}
        #${ZSTD}
        ${FOLLY}
        ${DOUBLECONV}
        ${GFLAGS}
        ${GLOG}
        ${ZMQ}
        ${SODIUM}
        ${PTHREAD}
        #${THRIFTPROTOCOL}
        #${PROTOCOL}
        #${TRANSPORT}
        )
