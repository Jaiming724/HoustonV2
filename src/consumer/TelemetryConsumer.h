#pragma once

#include <string>
#include <tuple>
#include "TypedConsumer.h"
#include "../dashboard/Dashboard.h"

class TelemetryConsumer : public TypedConsumer<std::tuple<std::string, std::string>> {
protected:
    std::tuple<std::string, std::string> decode(std::vector<uint8_t> &data) override {
        if (data.size() < sizeof(DashboardPacketHeader_t)) {
            return {"ERROR", "Incomplete Telemetry Data"};
        }

        DashboardPacketHeader_t header;
        std::memcpy(&header, data.data(), sizeof(DashboardPacketHeader_t));
        uint32_t startOfValue = sizeof(DashboardPacketHeader_t) + header.payloadKeySize;
        std::string key(data.begin() + sizeof(DashboardPacketHeader_t), data.begin() + startOfValue);
        switch (header.packetContentType) {
            case TYPE_UINT32:
                if (header.payloadValueSize != sizeof(uint32_t)) {
                    return {"ERROR", "Invalid Telemetry Value Size"};
                }
                {
                    uint32_t intValue;
                    std::memcpy(&intValue, data.data() + startOfValue, sizeof(uint32_t));
                    return {key, std::to_string(intValue)};
                }
            case TYPE_INT32:
                if (header.payloadValueSize != sizeof(int32_t)) {
                    return {"ERROR", "Invalid Telemetry Value Size"};
                }
                {
                    int32_t intValue;
                    std::memcpy(&intValue, data.data() + startOfValue, sizeof(int32_t));
                    return {key, std::to_string(intValue)};
                }
            case TYPE_FLOAT:
                if (header.payloadValueSize != sizeof(float)) {
                    return {"ERROR", "Invalid Telemetry Value Size"};
                }
                {
                    float floatValue;
                    std::memcpy(&floatValue, data.data() + startOfValue, sizeof(float));
                    return {key, std::to_string(floatValue)};
                }
            default: {
                std::string strValue(data.begin() + startOfValue,
                                     data.begin() + startOfValue + header.payloadValueSize);
                return {key, strValue};
            }
        }
    }
};