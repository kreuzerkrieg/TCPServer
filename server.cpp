#include <gflags/gflags.h>
#include <glog/logging.h>
#include <fbzmq/zmq/Message.h>
#include <iostream>
#include "ZConfig.h"
#include "ZServer.h"
#include "MQServerRunner.h"

int main(int argc, char **argv) {
    try {
        gflags::ParseCommandLineFlags(&argc, &argv, true);
        google::InitGoogleLogging(argv[0]);
        google::InstallFailureSignalHandler();

        auto lambda = [](auto &&msg) -> folly::Expected<fbzmq::Message, fbzmq::Error> {
            std::cout << "Request " << msg.data().data() << " received." << std::endl;
            return fbzmq::Message::wrapBuffer(folly::IOBuf::copyBuffer(msg.data(), msg.size()));
        };
        using functorType = decltype(lambda);

        MQConfig serverConfig;
        serverConfig.uri = "tcp://127.0.0.1:55556";
        MServer<functorType> server(std::move(serverConfig), std::move(lambda));
        ServerRunner<functorType> runner(server);
        while (true) { std::this_thread::yield(); }
    }
    catch (const std::exception &ex) {
        std::cout << "Server failed. Reason: " << ex.what() << std::endl;
    }
}