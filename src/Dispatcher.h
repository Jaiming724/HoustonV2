#pragma once

#include <unordered_map>
#include <string>
#include <optional>
#include <iostream>
#include <memory>
#include <utility>

#include "dashboard/Dashboard.h"
#include "consumer/DataConsumer.h"
#include "dashboard/COBS.h"

class Dispatcher {
private:
    std::unordered_map<uint8_t, DataConsumer *> handlerMap;
public:
    void registerHandler(uint8_t id, DataConsumer *handler) {
        handlerMap[id] = handler;
    }

    void dispatchData(std::vector<uint8_t> data) {
        if (data.size() < sizeof(DashboardPacketHeader_t)) {
            std::cerr << "Data too small to contain header" << std::endl;
            return;
        }
        DashboardPacketHeader_t header;
        uint32_t decodeSize;
        if (cobs_decode(data.data(), data.size(), &decodeSize) != COBS_DECODING_OK) {
            std::cerr << "COBS decoding failed" << std::endl;
            return;
        }
        if (decodeSize < sizeof(DashboardPacketHeader_t) + sizeof(DashboardPacketTail_t)) {
            std::cerr << "Decoded data too small to contain header and tail" << std::endl;
            return;
        }

        std::memcpy(&header, data.data(), sizeof(DashboardPacketHeader_t));
        if (header.magicNumber != DashboardMagicNumber) {
            std::cerr << "Invalid magic number" << std::endl;
            return;
        }
        if (crc32((char *) data.data(), sizeof(DashboardPacketHeader_t) - sizeof(uint32_t)) != header.checksum) {
            std::cerr << "Checksum mismatch" << std::endl;
            return;
        }
        uint32_t payloadSize = header.payloadKeySize + header.payloadValueSize;
        if (decodeSize != sizeof(DashboardPacketHeader_t) + payloadSize + sizeof(DashboardPacketTail_t)) {
            std::cerr << "Decoded data size mismatch" << std::endl;
            return;
        }
        DashboardPacketTail_t tail;
        std::memcpy(&tail, data.data() + sizeof(DashboardPacketHeader_t) + payloadSize, sizeof(DashboardPacketTail_t));
        if (crc32((char *) &data[sizeof(DashboardPacketHeader_t)], payloadSize) != tail.payloadChecksum) {
            std::cerr << "Payload checksum mismatch" << std::endl;
            return;
        }
        auto res = std::vector(data.begin() + sizeof(DashboardPacketHeader_t),
                               data.begin() + sizeof(DashboardPacketHeader_t) + payloadSize);
        getHandler(header.packetType)->consume(res);
        std::string key((char *) &data[sizeof(DashboardPacketHeader_t)], payloadSize);
        std::cout << "Dispatching data with key: " << key << std::endl;

    }

    void debugMap() {
        for (const auto &[id, handler]: handlerMap) {
            std::cout << "ID: " << (int) id << " -> ";
            if (handler == nullptr) {
                std::cout << "NULL (Danger!)" << std::endl;
            } else {
                std::cout << "Valid Object at " << handler << std::endl;
            }
        }

    }

    DataConsumer *getHandler(uint8_t key) {
        auto it = handlerMap.find(key);
        if (it != handlerMap.end()) {
            return it->second;
        }
        return nullptr;
    }

};