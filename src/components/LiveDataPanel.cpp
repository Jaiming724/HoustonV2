
#include "LiveDataPanel.h"
#include "../consumer/TypedConsumer.h"


LiveDataPanel::~LiveDataPanel() {

}

void LiveDataPanel::start() {
    refreshPackets();
}

void LiveDataPanel::refreshPackets() {
    liveDataMap.clear();
    intMap.clear();
    floatMap.clear();
    DashboardPacketHeader_t header;
    craftDashboardHeaderPacket(&header, ID_Request_LiveData, 0, 0, 0);
    DashboardPacketTail_t tail;
    tail.payloadChecksum = 0;
    std::vector<uint8_t> buffer;
    buffer.resize(sizeof(DashboardPacketHeader_t) + sizeof(DashboardPacketTail_t));
    std::memcpy(buffer.data(), &header, sizeof(DashboardPacketHeader_t));
    std::memcpy(buffer.data() + sizeof(DashboardPacketHeader_t), &tail, sizeof(DashboardPacketTail_t));
    dispatcher->sendData(std::move(buffer));
}

void LiveDataPanel::sendModifyPacket(uint8_t packetID, uint16_t valueType) {
    DashboardPacketHeader_t header;
    craftDashboardHeaderPacket(&header, ID_Modify, valueType, sizeof(LiveDataPacket_t), 0);

    LiveDataPacket_t payload;
    payload.packetID = packetID;
    payload.valueType = valueType;
    if (valueType == TYPE_FLOAT) {
        payload.floatValue = *floatMap[packetID];
    } else if (valueType == TYPE_BOOL) {
        payload.boolValue = *reinterpret_cast<bool *>(intMap[packetID]);
    } else {
        payload.int32Value = *intMap[packetID];
    }
    DashboardPacketTail_t tail;
    tail.payloadChecksum = crc32(reinterpret_cast<char *>(&payload), sizeof(LiveDataPacket_t), 0);
    std::vector<uint8_t> buffer;
    buffer.resize(sizeof(DashboardPacketHeader_t) + sizeof(LiveDataPacket_t) + sizeof(DashboardPacketTail_t));
    std::memcpy(buffer.data(), &header, sizeof(DashboardPacketHeader_t));
    std::memcpy(buffer.data() + sizeof(DashboardPacketHeader_t), &payload, sizeof(LiveDataPacket_t));
    std::memcpy(buffer.data() + sizeof(DashboardPacketHeader_t) + sizeof(LiveDataPacket_t), &tail,
                sizeof(DashboardPacketTail_t));
    dispatcher->sendData(std::move(buffer));
}

void LiveDataPanel::render() {
    DataConsumer *consumer = dispatcher->getHandler(ID_Response_LiveData);
    if (consumer) {
        auto *liveDataConsumer = static_cast<TypedConsumer<std::vector<LiveDataPacket_t>> *>(consumer);
        while (!liveDataConsumer->isEmpty()) {
            auto data = liveDataConsumer->pop();
            auto packet = *data;
            for (auto it: packet) {
                if (liveDataMap.find(it.packetID) == liveDataMap.end()) {
                    liveDataMap[it.packetID] = it;
                    if (it.valueType == TYPE_FLOAT) {
                        floatMap[it.packetID] = new float(it.floatValue);
                    } else {
                        intMap[it.packetID] = new int32_t(it.int32Value);
                    }
                }
            }
        }
    }
    ImGui::Begin("Live Data");
    for (const auto &it: liveDataMap) {

        if (it.second.valueType == TYPE_BOOL) {
            if (ImGui::Checkbox(std::to_string(it.first).c_str(), reinterpret_cast<bool *>(intMap[it.first]))) {
                sendModifyPacket(it.first, it.second.valueType);
            }
        } else {
            if (it.second.valueType == TYPE_FLOAT) {
                ImGui::InputFloat(std::to_string(it.first).c_str(), floatMap[it.first], 0.1f, 1.0f,
                                  "%.3f", 0);
            } else {
                ImGui::InputInt(std::to_string(it.first).c_str(), intMap[it.first], 1, 100, 0);
            }

            ImGui::SameLine();
            ImGui::PushID(it.first);
            if (ImGui::Button("Apply")) {
                sendModifyPacket(it.first, it.second.valueType);
            }
            ImGui::PopID();
        }
    }
    if (ImGui::Button("Refresh")) {
        refreshPackets();
    }
    ImGui::End();
}

void LiveDataPanel::stop() {
    liveDataMap.clear();
    intMap.clear();
    floatMap.clear();
}
