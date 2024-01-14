#pragma once

#include "pch.h"
#include "Setting.h"
#include "Util.h"

class ser;

class SerialHelper {
private:
    asio::io_service io;
    asio::serial_port port;
    //std::string *line = new std::string();
    long long start = -1;
    long count = 0;
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
        asio::write(port, asio::buffer(buf, 14));
        if (memcmp(modifyPacket->string_data, "BRE", 3) == 0 && modifyPacket->int_data == 1) {
            Setting::isEnable = false;
            close();
        }
    }

    void readAndPrintLines() {
        Setting::isEnableMutex.lock();

        if (!Setting::isEnable) {
            Setting::isEnableMutex.unlock();

            return;
        }
        auto currentTimePoint = std::chrono::system_clock::now();

        // Convert the time point to a duration since the epoch
        auto durationSinceEpoch = currentTimePoint.time_since_epoch();

        // Convert the duration to milliseconds
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(durationSinceEpoch);
        if(start == -1){
            start = milliseconds.count();
        }
        Setting::isEnableMutex.unlock();
        asio::streambuf buffer;
        std::size_t n = asio::read_until(port, buffer, '\n');
        asio::streambuf::const_buffers_type bufs = buffer.data();
        std::string line(asio::buffers_begin(bufs), asio::buffers_begin(bufs) + n);

        if (line.length() >= 4 && line.compare(0, 4, "CWC!", 0, 4) == 0) {
            Setting::telemetryMutex.lock();
            Setting::telemetryStr = std::string(line);
            Setting::telemetryMutex.unlock();
            count += 1;
            auto currentTimePoint = std::chrono::system_clock::now();

            // Convert the time point to a duration since the epoch
            auto durationSinceEpoch = currentTimePoint.time_since_epoch();

            // Convert the duration to milliseconds
            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(durationSinceEpoch);

            if (milliseconds.count() - start > 1000) {
                std::cout << "count: " << count << std::endl;
                count = 0;
                start = milliseconds.count();
            }
        } else if (line.length() >= 5 && line.compare(0, 5, "CWCA!", 0, 5) == 0) {
            Setting::alertMutex.lock();
            Setting::alertStr = std::string(line);
            Setting::alertMutex.unlock();
        } else if (line.length() >= 5 && line.compare(0, 5, "CWCM!", 0, 5) == 0) {
            Setting::modifyMutex.lock();
            Setting::modifyStr = std::string(line);
            Setting::modifyMutex.unlock();
        }
    }
};




