#include "TelemetryPanel.h"

void TelemetryPanel::start() {

}


void TelemetryPanel::render() {
    //auto currentTime = std::chrono::system_clock::now();

    //auto currentTimeMs = std::chrono::time_point_cast<std::chrono::milliseconds>(currentTime);

    // Get the actual time in milliseconds
    //auto timeMs = currentTimeMs.time_since_epoch().count();
    //std::cout<<timeMs<<" " <<Setting::telemetryStr<<std::endl;
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
        if (keySize == -1 && keys.size() > 0) {
            keySize = keys.size();
            data.clear();
            for (int i = 0; i < keys.size(); i++) {
                data.push_back(new Util::ScrollingBuffer());
            }
            showAnalog = new bool[keys.size()]{false};

        }
        if (keySize != keys.size()) {
            data.clear();
            for (int i = 0; i < keys.size(); i++) {
                data.push_back(new Util::ScrollingBuffer());
            }
            keySize = keys.size();
            delete[] showAnalog;
            showAnalog = new bool[keys.size()]{false};
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
    //ImPlot::ShowDemoWindow();
    ImGui::End();

}

void TelemetryPanel::stop() {

}

TelemetryPanel::~TelemetryPanel() {
    delete[] showAnalog;
}


void TelemetryPanel::graphData() {

    char label[32];
    for (int i = 0; i < keySize; i++) {
        ImGui::Checkbox(keys[i].c_str(), &showAnalog[i]);
        if (i < keySize - 1) {
            ImGui::SameLine();
        }
    }
    if (showAnalog == nullptr) {
        return;
    }
    static float t = 0;

    if (!paused) {
        t += ImGui::GetIO().DeltaTime;
        for (int i = 0; i < keySize; i++) {
            if (showAnalog[i]) {
                data[i]->AddPoint(t, std::stof(values[i]));
            }
        }
    }
    ImPlot::SetNextAxesToFit();

    if (ImPlot::BeginPlot("##Digital", ImVec2(-1, -1), ImPlotAxisFlags_AutoFit)) {
        ImPlot::SetupAxes(nullptr, nullptr );
        ImPlot::SetupAxisLimits(ImAxis_X1,t - 10, t, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1,0,1);

        for (int i = 0; i < keySize; ++i) {
            if (showAnalog[i]) {
                ImPlot::PlotLine(keys[i].c_str(), &data[i]->Data[0].x, &data[i]->Data[0].y, data[i]->Data.size(), 0, data[i]->Offset, 2*sizeof(float));

            }
        }
        ImPlot::EndPlot();
    }
}
