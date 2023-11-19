#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <asio.hpp>

class SerialHelper {
private:
    asio::io_service io;
    asio::serial_port port;
public:
    SerialHelper(const std::string &port_name) : io(), port(io, port_name) {

        port.set_option(asio::serial_port_base::baud_rate(9600));
        port.set_option(asio::serial_port_base::character_size(8));
        port.set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
        port.set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::none));

        readAndPrintLines();
    }

    virtual ~SerialHelper() {
        port.close();
    }

    void readAndPrintLines() {
        char c;
        std::string line;

        try {
            while (read(port, asio::buffer(&c, 1))) {
                if (c == '\n') {
                    // Process the complete line
                    std::cout << "Received line: " << line << std::endl;
                    line.clear();
                    break;
                } else {
                    // Append character to the current line
                    line += c;
                }
            }
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
};




