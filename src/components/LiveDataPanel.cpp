//
//#include "LiveDataPanel.h"
//
//LiveDataPanel::LiveDataPanel(const char *name, SerialHelper *serialHelper) : Component(name) {
//    this->serialHelper = serialHelper;
//}
//
//LiveDataPanel::~LiveDataPanel() {
//
//}
//
//void LiveDataPanel::start() {
//    Component::start();
//}
//
//void LiveDataPanel::render() {
//    //std::cout<<Setting::modifyStr<<std::endl;
//    if (Setting::modifyStr.length() >= 5) {
//
//        Setting::modifyMutex.lock();
//        std::string remainingString = Setting::modifyStr.substr(5);
//        Setting::modifyMutex.unlock();
//        std::vector<std::string> tokens = Util::splitString(remainingString, ';');
//        for (int i = 0; i < tokens.size() - 1; i++) {
//            std::vector<std::string> temp = Util::splitString(tokens[i], ':');
//            if (temp.size() != 2) {
//                continue;
//            }
//            if (dataMap.count(temp[0]) == 0) {
//                dataMap[temp[0]] = temp[1];
//            }
//            if (tokens[i].at(0) == 'I' && intMap.count(temp[0]) == 0) {
//                intMap[temp[0]] = new int(0);
//            } else if (tokens[i].at(0) == 'F' && floatMap.count(temp[0]) == 0) {
//                floatMap[temp[0]] = new float(0.0f);
//            } else if (tokens[i].at(0) == 'B' && boolMap.count(temp[0]) == 0) {
//                boolMap[temp[0]] = new bool(false);
//            }
//        }
//    }
//    ImGui::Begin("Live Data");
//    for (const auto &it: intMap) {
//
//        if (ImGui::InputInt(dataMap[it.first].c_str(), it.second, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
//            Util::ModifyPacket modifyPacket;
//            modifyPacket.int_data = *it.second;
//            memcpy(modifyPacket.string_data, it.first.c_str(), 3);
//            serialHelper->write(&modifyPacket);
//        }
//    }
//    for (const auto &it: floatMap) {
//        if (ImGui::InputFloat(dataMap[it.first].c_str(), it.second, 1, 100, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
//            Util::ModifyPacket modifyPacket;
//            modifyPacket.float_data = *it.second;
//            memcpy(modifyPacket.string_data, it.first.c_str(), 3);
//            serialHelper->write(&modifyPacket);
//        }
//    }
//    for (const auto &it: boolMap) {
//        if (ImGui::Checkbox(dataMap[it.first].c_str(), it.second)) {
//            Util::ModifyPacket modifyPacket;
//            modifyPacket.int_data = *it.second ? 1 : 0;
//            memcpy(modifyPacket.string_data, it.first.c_str(), 3);
//
//            serialHelper->write(&modifyPacket);
//        }
//    }
//
//    ImGui::End();
//}
//
//void LiveDataPanel::stop() {
//    Setting::modifyMutex.lock();
//    Setting::modifyStr = std::string("");
//    Setting::modifyMutex.unlock();
//    intMap.clear();
//    floatMap.clear();
//    boolMap.clear();
//}
