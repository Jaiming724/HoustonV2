#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <asio.hpp>
#include "Setting.h"
#include "Util.h"

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

        port.set_option(asio::serial_port_base::baud_rate(115200));
        port.set_option(asio::serial_port_base::character_size(8));
        port.set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
        port.set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::none));
    }

    void close() {
        port.close();
    }

    virtual ~SerialHelper() {
        std::cout << "SerialHelper::~SerialHelper()" << std::endl;
        port.close();
        delete line;
    }

    void write(Util::ModifyPacket *modifyPacket) {
        std::cout<<"write"<<std::endl;
        char temp[7] = {0};
        memcpy(temp, modifyPacket->string_data, 3);
        memcpy(temp + 3, &modifyPacket->int_data, 4);
        uint32_t checksum = Util::crc32(temp, 7);
        char buf[14] = {0};

        memcpy(buf, &modifyPacket->packet_id, 1);
        memcpy(buf + 1, &modifyPacket->packet_length, 2);
        memcpy(buf + 3, modifyPacket->string_data, 3);
        memcpy(buf + 6, &modifyPacket->int_data, 4);
        memcpy(buf + 10, &checksum, 4);
        asio::write(port, asio::buffer(buf, 14));
        if (memcmp(modifyPacket->string_data, "BRE", 3) == 0 && modifyPacket->int_data == 1) {
            Setting::isEnable = false;
            close();
        }
    }

    void readAndPrintLines() {
        char c;
        line->clear();
        try {
            while (read(port, asio::buffer(&c, 1))) {
                if (c == '\n') {
                    if (line->length() >= 4 && line->compare(0, 4, "CWC!", 0, 4) == 0) {
                        Setting::telemetryStr = std::string(*line);
                    } else if (line->length() >= 5 &&line->compare(0, 5, "CWCA!", 0, 5) == 0) {
                        Setting::alertStr = std::string(*line);
                    }else if(line->length() >= 5 && line->compare(0, 5, "CWCM!", 0, 5) == 0){
                        Setting::modifyStr =std::string(*line);
                    }
                    //std::cout << *line << std::endl;
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




