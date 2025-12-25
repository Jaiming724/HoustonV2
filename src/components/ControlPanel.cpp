
#include "ControlPanel.h"
#include "../producer/SerialProducer.h"

void ControlPanel::start() {

}

void ControlPanel::render() {
    ImGui::Begin("Control Panel");
    static char inputText[256] = "COM4"; // Buffer to store input text
    ImGui::InputText("Port", inputText, IM_ARRAYSIZE(inputText));
    char portNumber[20];

#ifdef _WIN32
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
    if (isEnable) {
        if (ImGui::Button("Detach")) {
            dataProducer->stop();

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
            dynamic_cast<SerialProducer *>(dataProducer)->setPort(inputText);
            for (auto &component: *pVector) {
                component->start();
            }
            isEnable = true;
        }
    }
    if (dataProducer != nullptr && dataProducer->status) {
        dataProducer->produce(dispatcher);
    }
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void ControlPanel::stop() {
}

ControlPanel::~ControlPanel() {

}
