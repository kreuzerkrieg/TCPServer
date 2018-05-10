#include <iostream>
#include <unordered_map>

#include <fbzmq/async/ZmqEventLoop.h>
#include <fbzmq/zmq/Zmq.h>
#include <fbzmq/zmq/Message.h>

void printMessage(folly::Expected<fbzmq::Message, fbzmq::Error> &msg, const char *file = __FILE__,
                  int line = __LINE__) {
    //std::cout << file << ":" << line;
//    if (msg.hasError()) {
//        std::cout << msg.error().errString << std::endl;
//    } else if (msg.hasValue()) {
//        std::cout << msg.value().read<std::string>().value() << std::endl;
//    }
}

struct MQConfig {
    std::chrono::milliseconds readTimeout = std::chrono::milliseconds(1000);
    std::chrono::milliseconds writeTimeout = std::chrono::milliseconds(1000);
    std::string uri;
    fbzmq::Context ctx;
    uint16_t maxIOThreads = 1;
    uint16_t maxSockets = 1024;
};

template<typename Func>
class MServer : public fbzmq::ZmqEventLoop {
public:

    MServer(MQConfig &&config, Func &&functor) : serverConfiguration(std::move(config)),
                                                 serverSocket(serverConfiguration.ctx/*, folly::none, folly::none,
                                                              fbzmq::NonblockingFlag{true}*/),
                                                 requestProcessor(std::move(functor)) {
        prepare();
    }

private:
    // Initialize ZMQ sockets
    void prepare() noexcept {
        serverSocket.bind(fbzmq::SocketUrl{serverConfiguration.uri}).value();

        // attach callbacks for multiple command
//        addSocket(fbzmq::RawZmqSocketPtr{*serverSocket}, ZMQ_POLLIN, [this](int) noexcept {
//            serverSocket.recvOne(serverConfiguration.readTimeout).then([this](auto &&msg) {
//                requestProcessor(msg).then([this](auto &&resp) {
//                    serverSocket.sendOne(resp);
//                });
//            });
//        });
        addSocket(fbzmq::RawZmqSocketPtr{*serverSocket}, ZMQ_POLLIN, [this](int) noexcept {
            auto msg = serverSocket.recvOne(serverConfiguration.readTimeout);
            printMessage(msg);
            auto resp = requestProcessor(msg);
            printMessage(resp);
            serverSocket.sendOne(resp.value());
        });
    }

    MQConfig serverConfiguration;
    fbzmq::Socket<ZMQ_REP, fbzmq::ZMQ_SERVER> serverSocket;
    Func requestProcessor;
};

template<typename T>
class ServerRunner {
public:
    ServerRunner(MServer<T> &srv) : server(srv) {

//    fbzmq::StopEventLoopSignalHandler handler(&mainEventLoop);
//    handler.registerSignalHandler(SIGINT);
//    handler.registerSignalHandler(SIGQUIT);
//    handler.registerSignalHandler(SIGTERM);

        // Zmq Context
        fbzmq::Context ctx;

        serverThread = std::thread([this]() noexcept {
            std::cout << "Starting Server thread ...";
            server.run();
            std::cout << "Server stopped.";
        });
        server.waitUntilRunning();
        eventThread = std::thread([this]() {
            std::cout << "Starting main event loop...";
            eventLoop.run();
            std::cout << "Main event loop got stopped";
        });
    }

    ~ServerRunner() {
        server.stop();
        server.waitUntilStopped();
        serverThread.join();

        eventLoop.stop();
        eventLoop.waitUntilStopped();
        eventThread.join();
    }

private:
    MServer<T> &server;
    std::thread serverThread;
    fbzmq::ZmqEventLoop eventLoop;
    std::thread eventThread;
};

class MClient {
public:
    MClient(MQConfig &&config) : clientConfiguration(std::move(config)),
                                 clientSocket(clientConfiguration.ctx/*, folly::none, folly::none,
                                              fbzmq::NonblockingFlag{true}*/) {
        prepare();
    }

//    auto sendClonedRequest(const void *data, uint64_t size) noexcept {
//        auto msg = fbzmq::Message::wrapBuffer(folly::IOBuf::copyBuffer(data, size)).value();
//        auto rc = clientSocket.sendOne(msg);
//        if (rc.hasError()) {
//            // TODO sending request failed"
//            //return rc;
//        }
//        return clientSocket.recvOne(clientConfiguration.readTimeout);
//    }
//
//    auto sendRequest(const void *data, uint64_t size) noexcept {
//        return fbzmq::Message::wrapBuffer(folly::IOBuf::wrapBuffer(data, size)).then([this](auto &&msg) {
//            auto rc = clientSocket.sendOne(std::move(msg));
//            if (rc.hasError()) {
//                // TODO sending request failed"
//                return rc;
//            }
//        }).then([this](auto &&res) { return clientSocket.recvOne(clientConfiguration.readTimeout); });
//    }
//
//    auto sendRequest(std::string &&str) noexcept {
//        return sendClonedRequest(static_cast<const void *>(str.data()), str.size());
//    }
//
//    template<typename T, std::enable_if_t<
//            std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value || std::is_same<T, char>::value ||
//            std::is_same<T, unsigned char>::value> * = nullptr>
//    auto sendRequest(std::vector<T> &&buff) noexcept {
//        return sendClonedRequest(static_cast<const void *>(buff.data()), buff.size());
//    }
//
//    auto sendRequest(const std::string &str) noexcept {
//        return sendRequest(static_cast<const void *>(str.data()), str.size());
//    }
//
//    template<typename T, std::enable_if_t<
//            std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value || std::is_same<T, char>::value ||
//            std::is_same<T, unsigned char>::value> * = nullptr>
//    auto sendRequest(const std::vector<T> &buff) noexcept {
//        return sendRequest(static_cast<const void *>(buff.data()), buff.size());
//    }

    auto sendClonedRequest(const void *data, uint64_t size) noexcept {
        auto msg = fbzmq::Message::wrapBuffer(folly::IOBuf::copyBuffer(data, size));
        auto rc = clientSocket.sendOne(msg.value());
        if (rc.hasError()) {
            // TODO sending request failed"
            //return rc;
        }
        return clientSocket.recvOne().value();
    }

    auto sendRequest(const void *data, uint64_t size) noexcept {
        return fbzmq::Message::wrapBuffer(folly::IOBuf::wrapBuffer(data, size)).then([this](auto &&msg) {
            auto rc = clientSocket.sendOne(std::move(msg));
            if (rc.hasError()) {
                // TODO sending request failed"
                return rc;
            }
        }).then([this](auto &&res) { return clientSocket.recvOne(clientConfiguration.readTimeout); });
    }

    auto sendRequest(std::string str) noexcept {
        auto msg = fbzmq::Message::from(str);
        printMessage(msg);
        auto rc = clientSocket.sendOne(msg.value());
        if (rc.hasError()) {
            // TODO sending request failed"
            //return rc;
        }
        auto resp = clientSocket.recvOne();
        printMessage(resp);
        return resp;
    }

    template<typename T, std::enable_if_t<
            std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value || std::is_same<T, char>::value ||
            std::is_same<T, unsigned char>::value> * = nullptr>
    auto sendRequest(std::vector<T> &&buff) noexcept {
        return sendClonedRequest(static_cast<const void *>(buff.data()), buff.size());
    }

private:
    // Initialize ZMQ sockets
    void prepare() noexcept {
        clientSocket.connect(fbzmq::SocketUrl{clientConfiguration.uri}).value();
    }


    MQConfig clientConfiguration;
    fbzmq::Socket<ZMQ_REQ, fbzmq::ZMQ_CLIENT> clientSocket;
};


int main(int argc, char **argv) {
    std::string s = "test";
    auto m = fbzmq::Message::from(s);
    printMessage(m);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
    // tcp://127.0.0.1:55556

    MQConfig serverConfig;
    serverConfig.uri = "tcp://127.0.0.1:55556";
    auto lambda = [](auto msg) {
        printMessage(msg);
        std::cout << "Request " << msg.value().data().data() << " received." << std::endl;
        return msg;
    };
    using functorType = decltype(lambda);
    MServer<functorType> server(std::move(serverConfig), std::move(lambda));
    ServerRunner<functorType> runner(server);

    MQConfig clientConfig1;
    clientConfig1.uri = "tcp://127.0.0.1:55556";
    MClient client1(std::move(clientConfig1));
//    MQConfig clientConfig2;
//    clientConfig2.uri = "tcp://127.0.0.1:55556";
//    MClient client2(std::move(clientConfig2));

    std::string str = "Hello";
    for (auto j = 0ul; j < 5; ++j) {
        for (auto i = 0ul; i < 5; ++i) {
            try {
                auto msg1 = str + "_" + std::to_string(i) + "_" + std::to_string(j) + "_client1";
                auto repMsg1 = client1.sendRequest(msg1);
                printMessage(repMsg1);
                std::cout << "Response " << repMsg1.value().data().data() << " received." << std::endl;
//                auto msg2 = str + "_" + std::to_string(i) + "_" + std::to_string(j) + "_client2";
//                auto repMsg2 = client2.sendRequest(std::move(msg2));
//                printMessage(repMsg2);
//                std::cout << "Response " << repMsg2.value().data().data() << " received." << std::endl;
            }
            catch (const std::exception &ex) {
                std::cout << "Exception thrown. Reason: " << ex.what() << std::endl;
            }
        }
    }
    return 0;
}