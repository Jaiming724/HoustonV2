#include <vector>
#include <iostream>
#include "TelemetryPanel.h"
#include "Setting.h"
#include "Util.h"

void TelemetryPanel::start() {

}

void TelemetryPanel::render() {
    if (Setting::serialStr.length() > 1 && Setting::serialStr.compare(0, 4, "CWC!", 0, 4) == 0) {
        str = Setting::serialStr;
    }

    // Create a window
    ImGui::Begin("2-Row Table Example");
    if (str.length() >= 4) {
        std::string remainingString = str.substr(4);
        std::vector<std::string> tokens = Util::splitString(remainingString, ';');
        ImGui::Columns(2, "Data");
        ImGui::Separator();

        for (auto &s: tokens) {
            std::vector<std::string> keyValue = Util::splitString(s, ':');
            if (keyValue.size() == 2) {
                ImGui::Text("%s", keyValue[0].c_str());
                //std::cout << keyValue[0] << std::endl;
                //std::cout << keyValue[1] << std::endl;

                ImGui::NextColumn();
                ImGui::Text("%s", keyValue[1].c_str());
                ImGui::NextColumn();
            }

        }

    }
    ImGui::End();

}

void TelemetryPanel::stop() {

}

TelemetryPanel::~TelemetryPanel() {

}