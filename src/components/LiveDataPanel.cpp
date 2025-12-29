
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
                liveDataMap[it.packetID] = it;
            }
        }
    }
    ImGui::Begin("Live Data");
    for (const auto &it: liveDataMap) {
        ImGui::Text("ID %d:", it.first);
//        if (ImGui::InputInt(std::to_string(it.first).c_str(), nullptr, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
//
//        }
    }
    if (ImGui::Button("Refresh")) {
        liveDataMap.clear();
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
}
