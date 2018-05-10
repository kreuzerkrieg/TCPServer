#include <iostream>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <thread>
#include "ZConfig.h"
#include "ZClient.h"

int main(int argc, char **argv) {
    MQConfig clientConfig1;
    clientConfig1.uri = "tcp://127.0.0.1:55556";
    MClient client1(std::move(clientConfig1));

    boost::uuids::random_generator gen;
    boost::uuids::uuid id = gen();
    auto clientId = boost::uuids::to_string(id);
    std::cout << "Client ID: " << clientId << std::endl;
    std::string str = "Hello";
    auto i = 0ul;
    while (true) {
        try {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            auto msg = str + "_" + std::to_string(i) + "_" + clientId;
            folly::Expected<fbzmq::Message, fbzmq::Error> resp = client1.sendRequest(std::move(msg));
            std::cout << "Response " << std::string(reinterpret_cast<const char *>(resp.value().data().data()),
                                                    resp.value().data().size()) << " received." << std::endl;
            ++i;
        }
        catch (const std::exception &ex) {
            std::cout << "Client failed. Reason: " << ex.what() << std::endl;
        }
    }
    return 0;
}