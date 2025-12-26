#include "AlertPanel.h"
#include "../consumer/TypedConsumer.h"

AlertPanel::~AlertPanel() {

}

void AlertPanel::start() {
    Component::start();
}

void AlertPanel::render() {
    DataConsumer *consumer = dispatcher->getHandler(ID_Alert);
    if (consumer) {
        auto *alertConsumer = static_cast<TypedConsumer<std::string> *>(consumer);
        while (!alertConsumer->isEmpty()) {
            std::optional<std::string> data = alertConsumer->pop();
            alerts.push_back(std::move(*data));
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

