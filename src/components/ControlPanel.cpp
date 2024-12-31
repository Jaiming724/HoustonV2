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
    char portNumber[20];
#ifdef IS_WINDOWS

    if (ImGui::Button("Detect Ports")) {
        HANDLE hSerial;

        for (int i = 1; i <= 256; i++) {
            std::sprintf(portNumber, "\\\\.\\com%d", i);

            hSerial = CreateFile(portNumber, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                 0);
            if (hSerial != INVALID_HANDLE_VALUE) {

                std::cout << "Detected port: COM" << i << std::endl;
            }
            CloseHandle(hSerial);

            memset(portNumber, 0, sizeof(portNumber));
        }

    }
#endif

    if (Setting::isEnable) {
        if (ImGui::Button("Detach")) {
            std::cout << "Detach" << std::endl;
            Setting::isEnableMutex.lock();
            Setting::isEnable = false;
            Setting::isEnableMutex.unlock();
            reader->close();
            for (auto &component: *pVector) {
                component->stop();
            }
        }
    } else {
        if (ImGui::Button("Attach")) {
            std::cout << "Attach" << std::endl;
            for (auto &component: *pVector) {
                component->start();
            }
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
