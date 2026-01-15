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
    std::vector<std::vector<uint8_t>> txData;
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
        if (crc16((char *) data.data(), sizeof(DashboardPacketHeader_t) - sizeof(header.checksum), 0) != header.checksum) {
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
        if (crc32((char *) &data[sizeof(DashboardPacketHeader_t)], payloadSize, 0) != tail.payloadChecksum) {
            std::cerr << "Payload checksum mismatch" << std::endl;
            return;
        }

        getHandler(header.packetType)->consume(std::move(data));
        std::cout << "Dispatching data with key: " << header.packetType << std::endl;
    }

    std::vector<std::vector<uint8_t>> &getTxData() {
        return txData;
    }

    void sendData(std::vector<uint8_t> data) {
        txData.push_back(std::move(data));
    }

    DataConsumer *getHandler(uint8_t key) {
        auto it = handlerMap.find(key);
        if (it != handlerMap.end()) {
            return it->second;
        }
        return nullptr;
    }

};