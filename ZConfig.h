#pragma once

#include <fbzmq/zmq/Context.h>

struct MQConfig {
    std::chrono::milliseconds readTimeout = std::chrono::milliseconds(100);
    std::chrono::milliseconds writeTimeout = std::chrono::milliseconds(100);
    std::string uri;
    fbzmq::Context ctx;
    uint16_t maxIOThreads = 1;
    uint16_t maxSockets = 1024;
};
