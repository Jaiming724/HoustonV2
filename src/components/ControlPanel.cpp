#include "ControlPanel.h"
#include "../Util.h"
#include "../producer/SerialProducer.h"

void ControlPanel::start() {
}

void ControlPanel::render() {
    ImGui::Begin("Control Panel");

    // Refresh ports button
    if (ImGui::Button("Refresh Ports") || availablePorts.empty()) {
        availablePorts = Util::getAvailablePorts();
        if (selectedPortIndex >= availablePorts.size()) {
            selectedPortIndex = 0;
        }
    }

    ImGui::SameLine();

    // Port selection dropdown
    if (!availablePorts.empty()) {
        if (ImGui::BeginCombo("Port", availablePorts[selectedPortIndex].c_str())) {
            for (int i = 0; i < availablePorts.size(); ++i) {
                bool isSelected = (selectedPortIndex == i);
                if (ImGui::Selectable(availablePorts[i].c_str(), isSelected)) {
                    selectedPortIndex = i;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    } else {
        ImGui::Text("No ports found");
    }

    //    static char inputText[32] = "COM5"; // Buffer to store input text
    //    ImGui::InputText("Port", inputText, IM_ARRAYSIZE(inputText));
    if (isEnable) {
        if (ImGui::Button("Detach")) {
            dataProducer->stop();
            delete dataProducer;
            dataProducer = nullptr;
            for (auto &component: *pVector) {
                component->stop();
            }
            isEnable = false;
        }
    } else {
        if (ImGui::Button("Attach")) {
            if (dataProducer == nullptr) {
                dataProducer = new SerialProducer();
            }
            std::string portName = "COM5";
            if (!availablePorts.empty() && selectedPortIndex < availablePorts.size()) {
                portName = availablePorts[selectedPortIndex];
            }
            dynamic_cast<SerialProducer *>(dataProducer)->setPort(portName);

            if (!dataProducer->start()) {
                delete dataProducer;
                dataProducer = nullptr;
                ImGui::Text("Failed to open port %s", portName.c_str());
                ImGui::End();
                return;
            }
            for (auto &component: *pVector) {
                component->start();
            }
            isEnable = true;
        }
    }
    if (dataProducer != nullptr && dataProducer->status) {
        dataProducer->produce(dispatcher);
        dataProducer->send_data(dispatcher);
    }
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImGui::End();
}

void ControlPanel::stop() {
}

ControlPanel::~ControlPanel() {
}
