//
// Created by Scratch on 11/19/2023.
//

#include "ControlPanel.h"
#include "../services/WebSocketProducer.h"
#include "AlertPanel.h"

void ControlPanel::start() {

}

void ControlPanel::render() {
    ImGui::Begin("Control Panel");

    ImGui::InputText("Address", addressBuf, IM_ARRAYSIZE(addressBuf));
    ImGui::InputText("Port", portBuf, IM_ARRAYSIZE(portBuf));

//#ifdef IS_WINDOWS
//
//    if (ImGui::Button("Detect Ports")) {
//        HANDLE hSerial;
//
//        for (int i = 1; i <= 256; i++) {
//            std::sprintf(portNumber, "\\\\.\\com%d", i);
//
//            hSerial = CreateFile(portNumber, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
//                                 0);
//            if (hSerial != INVALID_HANDLE_VALUE) {
//
//                std::cout << "Detected port: COM" << i << std::endl;
//            }
//            CloseHandle(hSerial);
//
//            memset(portNumber, 0, sizeof(portNumber));
//        }
//
//    }
//#endif
    if (enable) {
        if (ImGui::Button("Stop")) {

            dataProducer->stop();
            for (auto &component: *pVector) {
                component->stop();
            }
            enable = false;
            for (auto &component: *pVector) {
                if (strcmp(component->name, "Alert Panel") == 0) {
                    ((AlertPanel *) component)->alerts.push_back(
                            "Disconnected from " + std::string(addressBuf) + ":" + std::string(portBuf));
                }
            }
        }
    } else {
        if (ImGui::Button("Start")) {
            std::cout << "Attach" << std::endl;
            for (auto &component: *pVector) {
                component->start();
            }
            //TODO add a way to select the producer
            auto *wsProducer = dynamic_cast<WebSocketProducer *>(dataProducer);
            wsProducer->setAddress(addressBuf, portBuf);
            if (!wsProducer->start()) {
                std::cout << "Failed to start producer" << std::endl;
            } else {
                for (auto &component: *pVector) {
                    if (strcmp(component->name, "Alert Panel") == 0) {
                        ((AlertPanel *) component)->alerts.push_back(
                                "Connected to " + std::string(addressBuf) + ":" + std::string(portBuf));
                    }
                }
                enable = true;
            }
        }
    }
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void ControlPanel::stop() {
    enable = false;
    dataProducer->stop();
}

ControlPanel::~ControlPanel() {

}
