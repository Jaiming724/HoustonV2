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
    std::unordered_map<std::string, std::shared_ptr<DataConsumer>> handlerMap;
public:
    void registerHandler(const std::string &key, std::shared_ptr<DataConsumer> handler) {
        handlerMap[key] = std::move(handler);
    }

    void dispatchData(std::vector<uint8_t> data) {
        if (data.size() < sizeof(DashboardPacketHeader_t)) {
            std::cerr << "Data too small to contain header" << std::endl;
            return;
        }
        DashboardPacketHeader_t header;
        uint32_t decodeSize;
        if(cobs_decode(data.data(), data.size(), &decodeSize) != COBS_DECODING_OK){
            std::cerr << "COBS decoding failed" << std::endl;
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
        DashboardPacketTail_t tail;
        std::memcpy(&tail,data.data() + sizeof(DashboardPacketHeader_t) + payloadSize, sizeof(DashboardPacketTail_t));
        if (crc32((char *) &data[sizeof(DashboardPacketHeader_t)], payloadSize) != tail.payloadChecksum) {
            std::cerr << "Payload checksum mismatch" << std::endl;
            return;
        }
        std::string key((char *) &data[sizeof(DashboardPacketHeader_t)],
                        payloadSize);
        std::cout << "Dispatching data with key: " << key << std::endl;

    }

    std::shared_ptr<DataConsumer> getHandler(const std::string &key) {
        auto it = handlerMap.find(key);
        if (it != handlerMap.end()) {
            return it->second;
        }
        return nullptr;
    }

};