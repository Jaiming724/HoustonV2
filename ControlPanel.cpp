//
// Created by Scratch on 11/19/2023.
//

#include "ControlPanel.h"
#include "Setting.h"


void ControlPanel::start() {
}

void ControlPanel::render() {
    ImGui::Begin("Serial Port Example");
    static char inputText[256] = "Hello"; // Buffer to store input text
    ImGui::InputText("Enter Text", inputText, IM_ARRAYSIZE(inputText));
    if (Setting::isEnable) {
        if (ImGui::Button("Detach")) {
            Setting::isEnable = false;
            reader->close();
        }
    } else {
        if (ImGui::Button("Attach")) {
            Setting::isEnable = true;
            Setting::portName = inputText;
            reader->open(Setting::portName);
        }
    }
    ImGui::End();
}

void ControlPanel::stop() {
}

ControlPanel::~ControlPanel() {

}
