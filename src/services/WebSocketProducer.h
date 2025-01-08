#pragma once

#include "../pch.h"
#include "DataProducer.h"

class WebSocketProducer : public DataProducer {
private:
    const char *host;
    const char *port;
    int socket;
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::resolver resolver;
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws;
    boost::beast::flat_buffer buffer;
    std::function<void(const std::vector<uint8_t> &)> readCallback_;

public:
    WebSocketProducer(const char *name, Dispatcher *dispatcher, const char *host, const char *port)
            : DataProducer(name, dispatcher), resolver(ioc), ws(ioc) {
        this->host = host;
        this->port = port;
    };

    ~WebSocketProducer() override;

    void init() override;

    bool start() override;

    void fetch() override;

    void stop() override;

    void asyncWrite(const std::vector<uint8_t> &data);

    void asyncRead();

    void setReadCallback(std::function<void(const std::vector<uint8_t> &)> callback) {
        readCallback_ = std::move(callback);
    }

    void setAddress(const char *addr, const char *p) {
        this->host = addr;
        this->port = p;
    }

};