#pragma once

#include <iostream>
#include "ZServer.h"

template<typename T>
class ServerRunner {
public:
    ServerRunner(MServer<T> &srv) : server(srv) {

//        fbzmq::StopEventLoopSignalHandler handler(&eventLoop);
//        handler.registerSignalHandler(SIGINT);
//        handler.registerSignalHandler(SIGQUIT);
//        handler.registerSignalHandler(SIGTERM);

        // Zmq Context
        fbzmq::Context ctx;

        for (auto i = 0ul; i < 1; ++i) {
            eventThreads.emplace_back(std::thread([this]() noexcept {
                std::cout << "Starting Server thread ...";
                server.run();
                std::cout << "Server stopped.";
            }));
        }
        server.waitUntilRunning();
        eventThreads.emplace_back(std::thread([this]() {
            std::cout << "Starting main event loop...";
            eventLoop.run();
            std::cout << "Main event loop got stopped";
        }));
    }

    ~ServerRunner() {
        server.stop();
        server.waitUntilStopped();

        eventLoop.stop();
        eventLoop.waitUntilStopped();
        for (auto &&thread:eventThreads) {
            thread.join();
        }
    }

private:
    MServer<T> &server;
    fbzmq::ZmqEventLoop eventLoop;
    std::vector<std::thread> eventThreads;
};