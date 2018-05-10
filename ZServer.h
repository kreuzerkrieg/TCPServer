#pragma once

#include <fbzmq/async/ZmqEventLoop.h>
#include <fbzmq/zmq/Socket.h>
#include "ZConfig.h"

template<typename Func>
class MServer : public fbzmq::ZmqEventLoop {
public:

    MServer(MQConfig &&config, Func &&functor) : serverConfiguration(std::move(config)),
                                                 serverSocket(serverConfiguration.ctx),
                                                 requestProcessor(std::move(functor)) {
        prepare();
    }

private:
    // Initialize ZMQ sockets
    void prepare() noexcept {
        serverSocket.bind(fbzmq::SocketUrl{serverConfiguration.uri}).value();

        // attach callbacks for multiple command
        addSocket(fbzmq::RawZmqSocketPtr{*serverSocket}, ZMQ_POLLIN, [this](int) noexcept {
            try {
                serverSocket.recvOne(serverConfiguration.readTimeout).then([this](auto &&msg) {
                    requestProcessor(msg).then([this](auto &&repMsg) {
                        serverSocket.sendOne(repMsg);
                    });
                });
            }
            catch (const std::exception &ex) {
                std::string err = "Failed to send response. Reason: ";
                err += ex.what();
                try {
                    serverSocket.sendOne(fbzmq::Message::from(err).value());
                }
                catch (const std::exception &e) {
                    std::cout << "Failed to send response. Reason: " << ex.what() << ". Original reason: " << err
                              << std::endl;
                }
            }
        });
    }

    MQConfig serverConfiguration;
    fbzmq::Socket<ZMQ_DEALER, fbzmq::ZMQ_SERVER> serverSocket;
    Func requestProcessor;
};