#include "TelemetryPanel.h"
#include "../consumer/TelemetryConsumer.h"

void TelemetryPanel::start() {
    telemetryMap.clear();
}


void TelemetryPanel::render() {
    timer += ImGui::GetIO().DeltaTime * 1000; // Convert deltaTime to milliseconds
    DataConsumer *consumer = dispatcher->getHandler(ID_Telemetry);
    if (consumer) {
        auto *queueData = dynamic_cast<TypedConsumer<std::tuple<std::string, std::string>> *>(consumer);
        while (!queueData->isEmpty()) {
            auto [key, val] = queueData->pop().value();;
            telemetryMap[std::move(key)] = std::move(val);
        }
    }

    if (savingFile) {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto value = ms.time_since_epoch().count();
        file << value << ",";
        for (int i = 1; i < csvHeaders.size(); i++) {
            if (telemetryMap.count(csvHeaders[i]) > 0) {
                file << telemetryMap[csvHeaders[i]] << ",";
            } else {
                file << "-1,";
                continue;
            }
            //std::cout << telemetryMap[csvHeader] << ",";
        }
        file << std::endl;
        //std::cout << std::endl;
    }



    // Create a window
    ImGui::Begin("Telemetry");
    ImGui::Columns(2, "Data");
    ImGui::Separator();
    for (const auto &pair: telemetryMap) {
        ImGui::Text("%s", pair.first.c_str());
        ImGui::NextColumn();
        ImGui::Text("%s", pair.second.c_str());
        ImGui::NextColumn();
        ImGui::Separator();
    }

    ImGui::InputText("File", csvFileBuffer, IM_ARRAYSIZE(csvFileBuffer));
    if (savingFile) {
        if (ImGui::Button("Stop Saving File")) {
            savingFile = false;
            file.close();
        }
    } else {
        if (ImGui::Button("Save File")) {
            csvHeaders.clear();
            file.open(csvFileBuffer, std::ios_base::app); // Open the file in append mode
            if (!file.is_open()) {
                std::cerr << "Error: Unable to open file " << std::endl;
                return;
            }
            csvHeaders.emplace_back("Time");
            file << "Time,";
            for (auto it = dataMap.begin(); it != dataMap.end(); ++it) {
                csvHeaders.push_back(it->first);
                file << it->first << ",";
            }
            file << std::endl;

            savingFile = true;
        }
    }

    ImGui::End();
//    ImGui::Begin("Digital Plots");
//    graphData();
//    ImGui::End();

}

void TelemetryPanel::stop() {

    initalized = false;

    dataMap.clear();
    showMap.clear();
    savingFile = false;
    csvHeaders.clear();
    telemetryMap.clear();
    file.close();
}

TelemetryPanel::~TelemetryPanel() = default;


//void TelemetryPanel::graphData() {
//    if (!initalized) {
//        return;
//    }
//    for (const auto &pair: telemetryMap) {
//        ImGui::Checkbox(pair.first.c_str(), showMap[pair.first]);
//        ImGui::SameLine();
//    }
//    ImGui::Checkbox("Auto Scale", &autoScale);
//    ImGui::SameLine();
//    ImGui::SliderFloat("History", &history, 1, 60, "%.1f s");
//
//
//    static float t = 0;
//
//    if (!paused) {
//        t += ImGui::GetIO().DeltaTime;
//        for (const auto &pair: telemetryMap) {
//            if (*showMap[pair.first]) {
//                dataMap[pair.first]->AddPoint(t, std::stof(pair.second));
//            }
//        }
//    }
//    if (autoScale) {
//        ImPlot::SetNextAxesToFit();
//    }
//
//    if (ImPlot::BeginPlot("##Digital", ImVec2(-1, -1), ImPlotAxisFlags_AutoFit)) {
//        ImPlot::SetupAxes(nullptr, nullptr);
//        ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
//        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
//        for (const auto &pair: telemetryMap) {
//            if (*showMap[pair.first]) {
//                ImPlot::PlotLine(pair.first.c_str(), &dataMap[pair.first]->Data[0].x, &dataMap[pair.first]->Data[0].y,
//                                 dataMap[pair.first]->Data.size(), 0,
//                                 dataMap[pair.first]->Offset, 2 * sizeof(float));
//            }
//        }
//        ImPlot::EndPlot();
//    }
//}
