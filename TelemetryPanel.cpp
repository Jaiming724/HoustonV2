#include <vector>
#include <iostream>
#include "TelemetryPanel.h"
#include "Setting.h"
#include "Util.h"
#include "implot.h"

void TelemetryPanel::start() {

}


void TelemetryPanel::render() {
    if (Setting::serialStr.length() >= 4 && Setting::serialStr.compare(0, 4, "CWC!", 0, 4) == 0) {
        keys.clear();
        values.clear();
        std::string remainingString = Setting::serialStr.substr(4);
        std::vector<std::string> tokens = Util::splitString(remainingString, ';');
        for (auto &s: tokens) {
            std::vector<std::string> keyValue = Util::splitString(s, ':');
            if (keyValue.size() == 2) {
                keys.push_back(keyValue[0]);
                values.push_back(keyValue[1]);
            }
        }
        if (dataAnalog == nullptr || showAnalog == nullptr || keySize != keys.size()) {
            if (dataAnalog != nullptr) {
                delete dataAnalog;
                delete showAnalog;
            }
            dataAnalog = new Util::ScrollingBuffer[keys.size()];
            showAnalog = new bool[keys.size()]{false};
            keySize = keys.size();
        }

    }

    // Create a window
    ImGui::Begin("Telemetry");
    ImGui::Columns(2, "Data");
    ImGui::Separator();

    for (int i = 0; i < keys.size(); i++) {
        ImGui::Text("%s", keys[i].c_str());
        ImGui::NextColumn();
        ImGui::Text("%s", values[i].c_str());
        ImGui::NextColumn();
        ImGui::Separator();
    }
    ImGui::End();
    ImGui::Begin("Digital Plots");
    graphData();
    ImGui::End();

}

void TelemetryPanel::stop() {

}

TelemetryPanel::~TelemetryPanel() {
    delete dataAnalog;
    delete showAnalog;
}


void TelemetryPanel::graphData() {

    char label[32];
    for (int i = 0; i < keySize; i++) {
        ImGui::Checkbox(keys[i].c_str(), &showAnalog[i]);
        if (i < keySize - 1) {
            ImGui::SameLine();
        }
    }
    if (dataAnalog == nullptr || showAnalog == nullptr) {
        return;
    }
    static float t = 0;

    if (!paused) {
        t += ImGui::GetIO().DeltaTime;
        for (int i = 0; i < keySize; i++) {
            if (showAnalog[i]) {
                dataAnalog[i].AddPoint(t, std::stof(values[i]));
            }
        }
    }
    if (ImPlot::BeginPlot("##Digital",ImVec2(-1,-1),ImPlotAxisFlags_AutoFit)) {
        ImPlot::SetupAxisLimits(ImAxis_X1, t - 10.0, t, paused ? ImGuiCond_Once : ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 1);

        for (int i = 0; i < keySize; ++i) {
            if (showAnalog[i]) {
                snprintf(label, sizeof(label), "%s", keys[i].c_str());
                if (dataAnalog[i].Data.size() > 0)
                    ImPlot::PlotLine(label, &dataAnalog[i].Data[0].x, &dataAnalog[i].Data[0].y,
                                     dataAnalog[i].Data.size(), 0, dataAnalog[i].Offset, 2 * sizeof(float));
            }
        }

        ImPlot::EndPlot();
    }
}
