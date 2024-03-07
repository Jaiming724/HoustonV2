//
// Created by Scratch on 11/19/2023.
//

#include "ControlPanel.h"

void ControlPanel::start() {

}

#ifdef IS_WINDOWS

void ControlPanel::render() {
    ImGui::Begin("Control Panel");
    ImGui::ShowDemoWindow();
    static int itemCurrent = 0;
    //std::string portsList = "";
    //const char* portsList = "COM0\0COM1\0\0";
    //ImGui::InputText("Port", inputText, IM_ARRAYSIZE(inputText));
    if (ImGui::Button("Detect Ports")) {
        Setting::portsCombo = Util::ports(&Setting::portsList);

    }

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
            for (auto& component : *pVector) {
                component->start();
            }
            
            
            //std::vector<std::string> portNames = ports();
            //std::cout << "using " << portNames.front() << " instead of " << inputText << std::endl;
            Setting::portName = Setting::portsList[itemCurrent];
            reader->open(Setting::portName);
            Setting::isEnableMutex.lock();
            Setting::isEnable = true;
            std::cout << "Attaching port: " << Setting::portName << std::endl;
            Setting::isEnableMutex.unlock();
        }
    }
    ImGui::Combo("combo", &itemCurrent, Setting::portsCombo.c_str(), -1);
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}
#else
void ControlPanel::render() {
    ImGui::Begin("Control Panel");
    static char inputText[256] = "COM4"; // Buffer to store input text
    ImGui::InputText("Port", inputText, IM_ARRAYSIZE(inputText));
    if (Setting::isEnable) {
        if (ImGui::Button("Detach")) {
            std::cout << "Detach" << std::endl;
            Setting::isEnableMutex.lock();
            Setting::isEnable = false;
            Setting::isEnableMutex.unlock();
            reader->close();
            for (auto& component : *pVector) {
                component->stop();
            }
        }
    }
    else {
        if (ImGui::Button("Attach")) {
            std::cout << "Attach" << std::endl;
            for (auto& component : *pVector) {
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

#endif

/*
std::vector<std::string> ControlPanel::ports() {
    std::vector<std::string> stringVector;
    std::vector< tstring > ports;
    char port[20];
    HANDLE hSerial;

    for (int i = 1; i <= 256; i++)
    {
        int com_check_flag = 0;
        std::sprintf(port, "\\\\.\\com%d", i);

        hSerial = CreateFile(port, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (hSerial == INVALID_HANDLE_VALUE)
        {
            
            com_check_flag = 1;

        }
        else {

            std::string port = "COM" + std::to_string(i);
            std::cout << "Detected port: " << port << std::endl;

            stringVector.push_back(port);
            CloseHandle(hSerial);

        }
    }

    return stringVector;
}
*/
void ControlPanel::stop() {
}

ControlPanel::~ControlPanel() {

}
