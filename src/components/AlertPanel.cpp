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
    if (!Setting::alertQueue.empty()) {
        //std::cout<<"Debug::"<<Setting::alertStr<<std::endl;
        Setting::alertMutex.lock();
        while (!Setting::alertQueue.empty()) {
            std::string currentAlert = Setting::alertQueue.front();
            Setting::alertQueue.pop();
            std::string remainingString = currentAlert.substr(5);
            std::vector<std::string> tokens = Util::splitString(remainingString, ';');
            for (int i = 0; i < tokens.size() - 1; i++) {
                alerts.push_back(tokens.at(i));
            }
        }
        Setting::alertMutex.unlock();
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
    while (!Setting::alertQueue.empty()) {
        Setting::alertQueue.pop();
    }
    Setting::alertMutex.unlock();
}

