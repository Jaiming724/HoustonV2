//
// Created by Scratch on 11/20/2023.
//

#include "AlertPanel.h"
#include "../consumer/QueueData.h"


AlertPanel::~AlertPanel() {

}

void AlertPanel::start() {
    Component::start();
}

void AlertPanel::render() {
    DataConsumer *consumer = dispatcher->getHandler(ID_Alert);
    if (consumer) {
        auto *queueData = dynamic_cast<QueueData *>(consumer);
        while (!queueData->queue.empty()) {
            std::vector<uint8_t> &data = queueData->queue.front();
            alerts.emplace_back(data.begin(), data.end());
            queueData->queue.pop();
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
}

void AlertPanel::stop() {
    alerts.clear();
}

