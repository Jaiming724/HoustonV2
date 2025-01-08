
#include "LiveDataPanel.h"
#include "../services/consumer/QueueData.h"


LiveDataPanel::~LiveDataPanel() {

}

void LiveDataPanel::start() {
    Component::start();
}

void LiveDataPanel::render() {
    QueueData *queueData = (QueueData *) dispatcher->getHandler(std::string("LiveDataConsumer")).get();
    while (!queueData->queue.empty()) {
        auto val = queueData->queue.front();
        queueData->queue.pop();
        char type = val[1];
        std::string key(val.begin() + 2, val.begin() + 4);
        uint32_t result = 0;
        for (uint8_t i = 0; i < 4; i++) {
            uint8_t t = val[4 + i];
            result |= ((uint32_t) t) << i * 8;
        }
        //std::cout << "Received type " << type << " with key " << key << "with value " << result << std::endl;

        if (type == 'I' && intMap.count(key) == 0) {
            intMap[key] = new int(result);
        } else if (type == 'F' && floatMap.count(key) == 0) {
            floatMap[key] = new float((float) result);
        } else if (type == 'B' && boolMap.count(key) == 0) {
            boolMap[key] = new bool(result);
        }
    }
    ImGui::Begin("Live Data");

    for (const auto &it: intMap) {

        if (ImGui::InputInt(it.first.c_str(), it.second, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
            Util::ModifyPacket temp;
            temp.int_data = *it.second;
            memset(temp.string_data, 0, 3);
            memcpy(temp.string_data, "I", 1);
            memcpy(temp.string_data + 1, it.first.c_str(), 2);
            webSocketProducer->asyncWrite(Util::modifyPacketToVec(&temp));
        }
    }
    for (const auto &it: floatMap) {
        if (ImGui::InputFloat(it.first.c_str(), it.second, 1, 100, "%.3f",
                              ImGuiInputTextFlags_EnterReturnsTrue)) {
            Util::ModifyPacket temp;
            temp.float_data = *it.second;
            memset(temp.string_data, 0, 3);
            memset(temp.string_data, 'F', 1);
            memcpy(temp.string_data + 1, it.first.c_str(), 2);
            webSocketProducer->asyncWrite(Util::modifyPacketToVec(&temp));
        }
    }
    for (const auto &it: boolMap) {
        if (ImGui::Checkbox(it.first.c_str(), it.second)) {
            Util::ModifyPacket temp;
            temp.int_data = *it.second ? 1 : 0;
            memset(temp.string_data, 0, 3);
            memset(temp.string_data, 'B', 1);
            memcpy(temp.string_data + 1, it.first.c_str(), 2);
            webSocketProducer->asyncWrite(Util::modifyPacketToVec(&temp));
        }
    }

    ImGui::End();
}

void LiveDataPanel::stop() {
    Setting::modifyMutex.lock();
    Setting::modifyStr = std::string("");
    Setting::modifyMutex.unlock();
    intMap.clear();
    floatMap.clear();
    boolMap.clear();
}
