#pragma once

#include "pch.h"
#include "Setting.h"
#include "Util.h"

class SerialHelper {
private:
    boost::asio::io_context io;
    boost::asio::serial_port port;
    std::string *line = new std::string();

    long long start = -1;
    int count = 0;
public:
    SerialHelper() : io(), port(io) {
    }

    void open(const std::string &port_name) {
        port.open(port_name);

        port.set_option(boost::asio::serial_port_base::baud_rate(115200));
        port.set_option(boost::asio::serial_port_base::character_size(8));
        port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
        port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    }

    void close() {
        port.close();
    }

    virtual ~SerialHelper() {
        port.close();
        //delete line;
    }

    void write(Util::ModifyPacket *modifyPacket) {
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
        boost::asio::write(port, boost::asio::buffer(buf, 14));

    }

    void readAndPrintLines() {
        Setting::isEnableMutex.lock();

        if (!Setting::isEnable) {
            Setting::isEnableMutex.unlock();
            return;
        }
        Setting::isEnableMutex.unlock();
        auto currentTimePoint = std::chrono::system_clock::now();

        // Convert the time point to a duration since the epoch
        auto durationSinceEpoch = currentTimePoint.time_since_epoch();

        // Convert the duration to milliseconds
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(durationSinceEpoch);
        if (start == -1) {
            start = milliseconds.count();
        }

        char c;
        line->clear();
        try {
            while (read(port, boost::asio::buffer(&c, 1))) {
                if (c == '@') {
                    if (line->length() >= 4 && line->compare(0, 4, "CWC!", 0, 4) == 0) {
                        Setting::telemetryMutex.lock();
                        Setting::telemetryStr = std::string(*line);
                        Setting::telemetryMutex.unlock();

                        count += 1;
                        currentTimePoint = std::chrono::system_clock::now();
                        durationSinceEpoch = currentTimePoint.time_since_epoch();
                        milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(durationSinceEpoch);
                        if (milliseconds.count() - start > 1000) {
                            //std::cout << "count: " << count << std::endl;
                            count = 0;
                            start = milliseconds.count();
                        }
                        line->clear();
                    } else if (line->length() > 5 && line->compare(0, 5, "CWCA!", 0, 5) == 0) {
                        //std::cout << *line<<std::endl;

                        Setting::alertMutex.lock();
                        Setting::alertQueue.push(*line);
                        Setting::alertMutex.unlock();
                        line->clear();
                    } else if (line->length() >= 5 && line->compare(0, 5, "CWCM!", 0, 5) == 0) {
                        Setting::modifyMutex.lock();
                        Setting::modifyStr = std::string(*line);
                        Setting::modifyMutex.unlock();
                        line->clear();
                    }
                    break;
                } else {
                    *(line) += c;
                }

            }
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }
};




