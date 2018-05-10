#pragma once

#include <iostream>
#include "ZServer.h"

template<typename T>
class ServerRunner {
public:
    ServerRunner(MServer<T> &srv) : server(srv) {
        std::cout << "Starting server." << std::endl;
//        fbzmq::StopEventLoopSignalHandler handler(&eventLoop);
//        handler.registerSignalHandler(SIGINT);
//        handler.registerSignalHandler(SIGQUIT);
//        handler.registerSignalHandler(SIGTERM);

        // Zmq Context
        fbzmq::Context ctx;

        eventThreads.emplace_back(std::thread([this]() noexcept {
            std::cout << "Starting Server thread ..." << std::endl;
            server.run();
            std::cout << "Server stopped." << std::endl;
        }));
        server.waitUntilRunning();
        eventThreads.emplace_back(std::thread([this]() {
            std::cout << "Starting main event loop..." << std::endl;
            eventLoop.run();
            std::cout << "Main event loop got stopped" << std::endl;
        }));
        eventLoop.waitUntilRunning();
        std::cout << "Server is up and running" << std::endl;
    }

    ~ServerRunner() {
        std::cout << "Shutting the server down." << std::endl;
        server.stop();
        server.waitUntilStopped();

        eventLoop.stop();
        eventLoop.waitUntilStopped();
        for (auto &&thread:eventThreads) {
            thread.join();
        }
        std::cout << "Server is down." << std::endl;
    }

private:
    MServer<T> &server;
    fbzmq::ZmqEventLoop eventLoop;
    std::vector<std::thread> eventThreads;
};