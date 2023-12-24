//
// Created by Scratch on 11/19/2023.
//

#include "ControlPanel.h"

void ControlPanel::start() {
}

void ControlPanel::render() {
    ImGui::Begin("Control Panel");
    static char inputText[256] = "COM4"; // Buffer to store input text
    ImGui::InputText("Port", inputText, IM_ARRAYSIZE(inputText));
    if (Setting::isEnable) {
        if (ImGui::Button("Detach")) {
            reader->close();
            Setting::isEnableMutex.lock();
            Setting::isEnable = false;
            Setting::isEnableMutex.unlock();
        }
    } else {
        if (ImGui::Button("Attach")) {

            Setting::portName = inputText;
            reader->open(Setting::portName);
            Setting::isEnableMutex.lock();
            Setting::isEnable = true;
            Setting::isEnableMutex.unlock();
        }
    }
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void ControlPanel::stop() {
}

ControlPanel::~ControlPanel() {

}
