#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <asio.hpp>
#include "Setting.h"
class ser;

class SerialHelper {
private:
    asio::io_service io;
    asio::serial_port port;
    std::string *line = new std::string();

public:
    SerialHelper() : io(), port(io) {
    }

    void open(const std::string &port_name) {
        port.open(port_name);

        port.set_option(asio::serial_port_base::baud_rate(9600));
        port.set_option(asio::serial_port_base::character_size(8));
        port.set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
        port.set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::none));
    }

    void close() {
        port.close();
    }

    virtual ~SerialHelper() {
        std::cout<<"SerialHelper::~SerialHelper()"<<std::endl;
        port.close();
        delete line;
    }

    void readAndPrintLines() {
        char c;
        line->clear();
        try {
            while (read(port, asio::buffer(&c, 1))) {
                if (c == '\n') {
                    Setting::serialStr = *line;
                    break;
                } else {
                    *(line) += c;
                }
            }
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
};




