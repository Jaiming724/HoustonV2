#include "TelemetryPanel.h"

void TelemetryPanel::start() {

}


void TelemetryPanel::render() {
    if (Setting::telemetryStr.length() >= 4) {
        keys.clear();
        values.clear();
        Setting::telemetryMutex.lock();
        std::string remainingString = Setting::telemetryStr.substr(4);
        Setting::telemetryMutex.unlock();
        std::vector<std::string> tokens = Util::splitString(remainingString, ';');
        for (auto &s: tokens) {
            std::vector<std::string> keyValue = Util::splitString(s, ':');
            if (keyValue.size() == 2) {
                keys.push_back(keyValue[0]);
                values.push_back(keyValue[1]);
            }
        }
        if (!initalized && !keys.empty()) {
            std::cout << "init with size" << keys.size() << std::endl;
            initalized = true;
            data.clear();
            showAnalog.clear();
            for (int i = 0; i < keys.size(); i++) {
                data.push_back(new Util::ScrollingBuffer());
                showAnalog.push_back(new bool(false));
            }
            keysize = keys.size();
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

    initalized = false;

    Setting::telemetryMutex.lock();
    Setting::telemetryStr = std::string("");
    Setting::telemetryMutex.unlock();
    data.clear();
    showAnalog.clear();
    keysize = -1;
}

TelemetryPanel::~TelemetryPanel() {
}


void TelemetryPanel::graphData() {
    if (!initalized || keysize == -1 || keys.size() != keysize) {
        return;
    }
    for (int i = 0; i <keysize; i++) {
        ImGui::Checkbox(keys[i].c_str(), showAnalog[i]);
        ImGui::SameLine();

    }
    ImGui::SliderFloat("History", &history, 1, 60, "%.1f s");
    ImGui::SameLine();
    ImGui::Checkbox("Auto Scale", &autoScale);

    static float t = 0;

    if (!paused) {
        t += ImGui::GetIO().DeltaTime;
        for (int i = 0; i <keysize; i++) {
            if (*showAnalog[i]) {
                data[i]->AddPoint(t, std::stof(values[i]));
            }
        }
    }
    if (autoScale) {
        ImPlot::SetNextAxesToFit();
    }

    if (ImPlot::BeginPlot("##Digital", ImVec2(-1, -1), ImPlotAxisFlags_AutoFit)) {
        ImPlot::SetupAxes(nullptr, nullptr);
        ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);

        for (int i = 0; i < keysize; ++i) {
            if (*showAnalog[i]) {
                ImPlot::PlotLine(keys[i].c_str(), &data[i]->Data[0].x, &data[i]->Data[0].y, data[i]->Data.size(), 0,
                                 data[i]->Offset, 2 * sizeof(float));

            }
        }
        ImPlot::EndPlot();
    }
}
