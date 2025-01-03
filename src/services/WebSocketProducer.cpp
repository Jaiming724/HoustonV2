//
// Created by Jiami on 11/27/2024.
//

#include "WebSocketProducer.h"

void WebSocketProducer::init() {

}

void WebSocketProducer::start() {
    try {
        this->status = true;
        auto const results = resolver.resolve(host, port);
        for (const auto &endpoint: results) {
            std::cout << "IP: " << endpoint.endpoint().address()
                      << ", Port: " << endpoint.endpoint().port() << std::endl;
        }
        boost::asio::connect(ws.next_layer(), results.begin(), results.end());
        ws.handshake(host, "/ws");
        std::cout << "Connected to WebSocket server." << std::endl;
        asyncRead();
        std::vector<uint8_t> binaryData = {0xAA, 0xBB, 0xCC, 0xDD, 0xAA, 0xBB};

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

}

void WebSocketProducer::fetch() {

    ioc.poll();
}

void WebSocketProducer::asyncWrite(const std::vector<uint8_t> &data) {
    auto buf = boost::asio::buffer(data);
    ws.async_write(
            buf,
            [this](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (ec) {
                    std::cerr << "WebSocket write error: " << ec.message() << std::endl;
                    return;
                }

                std::cout << "Successfully sent " << bytes_transferred << " bytes." << std::endl;
            });

}

void WebSocketProducer::asyncRead() {
    ws.async_read(
            buffer,
            [this](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (ec) {
                    if (ec == boost::beast::websocket::error::closed) {
                        std::cout << "WebSocket connection closed by server." << std::endl;
                    } else {
                        std::cerr << "WebSocket read error: " << ec.message() << std::endl;
                    }
                    return;
                }

                auto data = boost::asio::buffer_cast<const uint8_t *>(buffer.data());
                std::vector<uint8_t> receivedData(data, data + bytes_transferred);
                readCallback_(receivedData);
                buffer.consume(bytes_transferred);
                if (this->status) {
                    asyncRead();
                }
            });
}

void WebSocketProducer::stop() {
    this->status = false;
    try {
        if (ws.is_open()) {
            ws.close(boost::beast::websocket::close_code::normal);
        }
    } catch (const std::exception &e) {
        std::cerr << "Error during WebSocket closure: " << e.what() << std::endl;
    }
    ioc.stop(); // Ensure io_context stops all work
}

WebSocketProducer::~WebSocketProducer() {

}