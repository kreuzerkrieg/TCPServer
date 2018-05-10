#include <thread>
#include "ZClient.h"

MClient::MClient(MQConfig &&config) : clientConfiguration(std::move(config)),
                                      clientSocket(clientConfiguration.ctx) {
    clientSocket.connect(fbzmq::SocketUrl{clientConfiguration.uri}).value();
}

folly::Expected<fbzmq::Message, fbzmq::Error> MClient::sendRequest(const void *data, uint64_t size) noexcept {
    return fbzmq::Message::wrapBuffer(folly::IOBuf::wrapBuffer(data, size)).then([this](auto &&msg) {
        auto rc = clientSocket.sendOne(std::move(msg));
        if (rc.hasError()) {
            // TODO sending request failed"
            //return rc;
        }
    }).then([this](auto &&res) { return clientSocket.recvOne(clientConfiguration.readTimeout); }).value();
}

folly::Expected<fbzmq::Message, fbzmq::Error> MClient::sendClonedRequest(const void *data, uint64_t size) noexcept {
    return fbzmq::Message::wrapBuffer(folly::IOBuf::copyBuffer(data, size)).then([this](auto &&msg) {
        auto rc = clientSocket.sendOne(std::move(msg));
        if (rc.hasError()) {
            // TODO sending request failed"
            //return rc;
        }
    }).then([this](auto &&res) { return clientSocket.recvOne(clientConfiguration.readTimeout); }).value();
}

folly::Expected<fbzmq::Message, fbzmq::Error> MClient::sendRequest(const std::string &str) noexcept {
    return sendRequest(static_cast<const void *>(str.data()), str.size());
}

folly::Expected<fbzmq::Message, fbzmq::Error> MClient::sendRequest(std::string &&str) noexcept {
    return sendClonedRequest(static_cast<const void *>(str.data()), str.size());
}
