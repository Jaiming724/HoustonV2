
#include "LiveDataPanel.h"

LiveDataPanel::LiveDataPanel(const char *name, SerialHelper *serialHelper) : Component(name) {
    this->serialHelper = serialHelper;
}

LiveDataPanel::~LiveDataPanel() {

}

void LiveDataPanel::start() {
    Component::start();
}

void LiveDataPanel::render() {
    //std::cout<<Setting::modifyStr<<std::endl;
    if (Setting::modifyStr.length() >= 5) {
        Setting::modifyMutex.lock();
        std::string remainingString = Setting::modifyStr.substr(5);
        Setting::modifyMutex.unlock();
        std::vector<std::string> tokens = Util::splitString(remainingString, ';');
        for (int i = 0; i < tokens.size() - 1; i++) {
            if (tokens[i].at(0) == 'I' && intMap.count(tokens[i]) == 0) {
                intMap[tokens[i]] = new int(0);
            } else if (tokens[i].at(0) == 'F' && floatMap.count(tokens[i]) == 0) {
                floatMap[tokens[i]] = new float(0.0f);
            } else if (tokens[i].at(0) == 'B' && boolMap.count(tokens[i]) == 0) {
                boolMap[tokens[i]] = new bool(false);
            }
        }
    }
    ImGui::Begin("Live Data");
    for (const auto &it: intMap) {
        if (ImGui::InputInt(it.first.c_str(), it.second, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
            Util::ModifyPacket modifyPacket;
            modifyPacket.int_data = *it.second;
            memcpy(modifyPacket.string_data, it.first.c_str(), 3);
            serialHelper->write(&modifyPacket);
        }
    }
    for (const auto &it: floatMap) {
        if (ImGui::InputFloat(it.first.c_str(), it.second, 1, 100, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
            Util::ModifyPacket modifyPacket;
            modifyPacket.float_data = *it.second;
            memcpy(modifyPacket.string_data, it.first.c_str(), 3);
            serialHelper->write(&modifyPacket);
        }
    }
    for (const auto &it: boolMap) {
        if (ImGui::Checkbox(it.first.c_str(), it.second)) {
            Util::ModifyPacket modifyPacket;
            modifyPacket.int_data = *it.second ? 1 : 0;
            memcpy(modifyPacket.string_data, it.first.c_str(), 3);

            serialHelper->write(&modifyPacket);
        }
    }

    ImGui::End();
}

void LiveDataPanel::stop() {
    Component::stop();
}
