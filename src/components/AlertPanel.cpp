//
// Created by Scratch on 11/20/2023.
//

#include "AlertPanel.h"


AlertPanel::~AlertPanel() {

}

void AlertPanel::start() {
    Component::start();
}

void AlertPanel::render() {
    if (Setting::alertStr.length() >= 5) {
        Setting::alertMutex.lock();
        std::string remainingString = Setting::alertStr.substr(5);
        Setting::alertMutex.unlock();
        std::vector<std::string> tokens = Util::splitString(remainingString, ';');
        for (int i = 0; i < tokens.size()-1; i++) {
            alerts.push_back(tokens.at(i));
        }
        Setting::alertStr = "";
    }

    ImGui::Begin("Alerts");
    for (auto &s: alerts) {
        ImGui::Text("%s", s.c_str());
    }
    if (ImGui::Button("Clear")) {
        alerts.clear();
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::End();
}

void AlertPanel::stop() {
    alerts.clear();
    Setting::alertMutex.lock();
    Setting::alertStr = std::string("");
    Setting::alertMutex.unlock();
}

