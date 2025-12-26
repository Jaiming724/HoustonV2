#pragma once

#include "TypedConsumer.h"
#include "../dashboard/Dashboard.h"

class AlertConsumer : public TypedConsumer<std::string> {
public:
protected:
    std::string decode(std::vector<uint8_t> &data) override {
        if (data.size() < sizeof(DashboardPacketHeader_t)) {
            return "ERROR: Incomplete Alert";
        }

        DashboardPacketHeader_t header;
        std::memcpy(&header, data.data(), sizeof(DashboardPacketHeader_t));

        uint32_t expectedEnd = sizeof(DashboardPacketHeader_t) + header.payloadValueSize;
        if (data.size() < expectedEnd) {
            return "ERROR: Incomplete Alert";
        }

        return {
                data.begin() + sizeof(DashboardPacketHeader_t),
                data.begin() + expectedEnd
        };
    }

};