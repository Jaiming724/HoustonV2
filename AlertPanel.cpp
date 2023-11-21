//
// Created by Scratch on 11/20/2023.
//

#include "AlertPanel.h"
#include "Setting.h"
#include "Util.h"

AlertPanel::~AlertPanel() {

}

void AlertPanel::start() {
    Component::start();
}

void AlertPanel::render() {
    if (Setting::serialStr.length() >= 5 && Setting::serialStr.compare(0, 5, "CWCA!", 0, 5) == 0) {
        std::string remainingString = Setting::serialStr.substr(5);
        std::vector<std::string> tokens = Util::splitString(remainingString, ';');
        for (int i = 0; i < tokens.size()-1; i++) {
            alerts.push_back(tokens.at(i));
        }
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
    ImGui::ShowDemoWindow();
}

void AlertPanel::stop() {
    Component::stop();
}

