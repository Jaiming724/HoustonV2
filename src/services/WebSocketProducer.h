#pragma once

#include "../pch.h"
#include "DataProducer.h"

class WebSocketProducer : public DataProducer {
private:
    const char *host;
    const char* port;
    int socket;
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::resolver resolver;
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws;
    boost::beast::flat_buffer buffer;

public:
    WebSocketProducer(const char *name, const char *host, const char* port)
            : DataProducer(name), resolver(ioc), ws(ioc) {
        this->host = host;
        this->port = port;
    };

    ~WebSocketProducer() override;

    void init() override;
    void start() override;
    void fetch() override;

    void stop() override;

    void asyncRead();
};