#pragma once

#include <cstdint>
#include <fbzmq/zmq/Socket.h>
#include "ZConfig.h"

class MQConfig;

class MClient {
public:
    MClient(MQConfig &&config);

    folly::Expected<fbzmq::Message, fbzmq::Error> sendClonedRequest(const void *data, uint64_t size) noexcept;

    folly::Expected<fbzmq::Message, fbzmq::Error> sendRequest(const void *data, uint64_t size) noexcept;

    folly::Expected<fbzmq::Message, fbzmq::Error> sendRequest(std::string &&str) noexcept;

    template<typename T, std::enable_if_t<
            std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value || std::is_same<T, char>::value ||
            std::is_same<T, unsigned char>::value> * = nullptr>
    folly::Expected<fbzmq::Message, fbzmq::Error> sendRequest(std::vector <T> &&buff) noexcept {
        return sendClonedRequest(static_cast<const void *>(buff.data()), buff.size());
    }

    folly::Expected<fbzmq::Message, fbzmq::Error> sendRequest(const std::string &str) noexcept;

    template<typename T, std::enable_if_t<
            std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value || std::is_same<T, char>::value ||
            std::is_same<T, unsigned char>::value> * = nullptr>
    folly::Expected<fbzmq::Message, fbzmq::Error> sendRequest(const std::vector <T> &buff) noexcept {
        return sendRequest(static_cast<const void *>(buff.data()), buff.size());
    }

private:
    MQConfig clientConfiguration;
    fbzmq::Socket <ZMQ_DEALER, fbzmq::ZMQ_CLIENT> clientSocket;
};