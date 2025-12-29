
#include "LiveDataPanel.h"
#include "../consumer/TypedConsumer.h"


LiveDataPanel::~LiveDataPanel() {

}

void LiveDataPanel::start() {
    Component::start();
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
        if (it.second.valueType == TYPE_FLOAT) {
            ImGui::InputFloat(std::to_string(it.first).c_str(), floatMap[it.first], 0.1f, 1.0f,
                                  "%.3f", 0);


        } else if (it.second.valueType == TYPE_BOOL) {
            ImGui::Checkbox(std::to_string(it.first).c_str(), reinterpret_cast<bool *>(intMap[it.first]));
        } else {
            ImGui::InputInt(std::to_string(it.first).c_str(), intMap[it.first], 1, 100,
                            0);

        }
        ImGui::PushID(it.first);
        if(ImGui::Button("Apply")){
            DashboardPacketHeader_t header;
            header.magicNumber = DashboardMagicNumber;
            header.packetType = ID_Modify;
            header.packetContentType = it.second.valueType;
            header.timestamp = 0;
            header.payloadKeySize = sizeof(LiveDataPacket_t);
            header.payloadValueSize = 0;
            LiveDataPacket_t payload;
            payload.packetID = it.first;
            payload.valueType = it.second.valueType;
            if (it.second.valueType == TYPE_FLOAT) {
                payload.floatValue = *floatMap[it.first];
            } else if (it.second.valueType == TYPE_BOOL) {
                payload.boolValue = *reinterpret_cast<bool *>(intMap[it.first]);
            } else {
                payload.int32Value = *intMap[it.first];
            }
            header.checksum = crc32(reinterpret_cast<char *>(&header),sizeof(DashboardPacketHeader_t) - sizeof(uint32_t), 0);
            DashboardPacketTail_t tail;
            tail.payloadChecksum = crc32(reinterpret_cast<char *>(&payload), sizeof(LiveDataPacket_t), 0);
            std::vector<uint8_t> buffer;
            buffer.resize(sizeof(DashboardPacketHeader_t) + sizeof(LiveDataPacket_t) + sizeof(DashboardPacketTail_t));
            std::memcpy(buffer.data(), &header, sizeof(DashboardPacketHeader_t));
            std::memcpy(buffer.data() + sizeof(DashboardPacketHeader_t), &payload, sizeof(LiveDataPacket_t));
            std::memcpy(buffer.data() + sizeof(DashboardPacketHeader_t) + sizeof(LiveDataPacket_t), &tail, sizeof(DashboardPacketTail_t));
            dispatcher->sendData(std::move(buffer));
        }
        ImGui::PopID();

    }
    if (ImGui::Button("Refresh")) {
        liveDataMap.clear();
        intMap.clear();
        floatMap.clear();
        DashboardPacketHeader_t header;
        header.magicNumber = DashboardMagicNumber;
        header.packetType = ID_Request_LiveData;
        header.packetContentType = 0;
        header.timestamp = 0;
        header.payloadKeySize = 0;
        header.payloadValueSize = 0;
        header.checksum = crc32(reinterpret_cast<char *>(&header),
                                sizeof(DashboardPacketHeader_t) - sizeof(uint32_t), 0);
        DashboardPacketTail_t tail;
        tail.payloadChecksum = 0;
        std::vector<uint8_t> buffer;
        buffer.resize(sizeof(DashboardPacketHeader_t) + sizeof(DashboardPacketTail_t));
        std::memcpy(buffer.data(), &header, sizeof(DashboardPacketHeader_t));
        std::memcpy(buffer.data() + sizeof(DashboardPacketHeader_t), &tail, sizeof(DashboardPacketTail_t));
        dispatcher->sendData(std::move(buffer));
    }
    ImGui::End();
}

void LiveDataPanel::stop() {
    liveDataMap.clear();
    intMap.clear();
    floatMap.clear();
}
