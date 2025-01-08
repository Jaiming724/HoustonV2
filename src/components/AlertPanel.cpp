//
// Created by Scratch on 11/20/2023.
//

#include "AlertPanel.h"
#include "../services/consumer/QueueData.h"


AlertPanel::~AlertPanel() {

}

void AlertPanel::start() {
    Component::start();
}

void AlertPanel::render() {
    QueueData *queueData = (QueueData *) dispatcher->getHandler(std::string("AlertConsumer")).get();
    while (!queueData->queue.empty()) {
        auto val = queueData->queue.front();
        std::string str(val.begin() + 2, val.begin() + 4);

        uint32_t intValue = 0;
        std::memcpy(&intValue, &val[4], sizeof(intValue));

        // Step 3: Append the integer to the string
        str += " is set to value: " + std::to_string(intValue);

        alerts.push_back(str);
        queueData->queue.pop();
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

