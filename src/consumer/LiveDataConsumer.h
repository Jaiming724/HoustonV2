#pragma once

#include "../dashboard/Dashboard.h"
#include "TypedConsumer.h"

class LiveDataConsumer : public TypedConsumer<std::vector<LiveDataPacket_t>> {
protected:
    std::vector<LiveDataPacket_t> decode(std::vector<uint8_t> &data) override {
        std::vector<LiveDataPacket_t> packets;
        if (data.size() < sizeof(DashboardPacketHeader_t)) {
            return packets;
        }
        DashboardPacketHeader_t header;
        std::memcpy(&header, data.data(), sizeof(DashboardPacketHeader_t));
        for (int i = 0; i < header.payloadKeySize; i++) {
            LiveDataPacket_t packet;
            size_t offset = sizeof(DashboardPacketHeader_t) + (i * sizeof(LiveDataPacket_t));
            if (offset + sizeof(LiveDataPacket_t) <= data.size()) {
                std::memcpy(&packet, data.data() + offset, sizeof(LiveDataPacket_t));
                packets.push_back(packet);
            }
        }
        return packets;
    }
};


