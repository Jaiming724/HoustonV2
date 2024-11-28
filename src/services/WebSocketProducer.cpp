//
// Created by Jiami on 11/27/2024.
//

#include "WebSocketProducer.h"

void WebSocketProducer::init() {

}

void WebSocketProducer::start() {
    try {
        auto const results = resolver.resolve(host, port);
        for (const auto &endpoint: results) {
            std::cout << "IP: " << endpoint.endpoint().address()
                      << ", Port: " << endpoint.endpoint().port() << std::endl;
        }
        boost::asio::connect(ws.next_layer(), results.begin(), results.end());
        ws.handshake(host, "/ws");
        std::cout << "Connected to WebSocket server." << std::endl;
        asyncRead();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void WebSocketProducer::fetch() {

    ioc.poll();
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

                // Process the received message
                std::cout << "Received Data:" << std::endl;
                auto data = boost::asio::buffer_cast<const uint8_t*>(buffer.data());
                for (std::size_t i = 0; i < bytes_transferred; ++i) {
                    std::cout << std::hex << static_cast<int>(data[i]) << " ";
                }
                std::cout << std::endl;

                // Clear the buffer for the next read
                buffer.consume(bytes_transferred);

                // Schedule another asynchronous read
                asyncRead();
            });
}

void WebSocketProducer::stop() {
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
