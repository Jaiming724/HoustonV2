#include "TelemetryPanel.h"

void TelemetryPanel::start() {
    telemetryMap.clear();
}


void TelemetryPanel::render() {
    timer += ImGui::GetIO().DeltaTime * 1000; // Convert deltaTime to milliseconds

    if (Setting::telemetryStr.length() >= 4 ) {
        timer=0;
        initalized = true;
//        keys.clear();
//        values.clear();
        telemetryMap.clear();
        Setting::telemetryMutex.lock();
        std::string remainingString = Setting::telemetryStr.substr(4);
        Setting::telemetryMutex.unlock();
        std::vector<std::string> tokens = Util::splitString(remainingString, ';');
        for (auto &s: tokens) {
            std::vector<std::string> keyValue = Util::splitString(s, ':');
            if (keyValue.size() == 2) {

//                keys.push_back(keyValue[0]);
//                values.push_back(keyValue[1]);
                telemetryMap[keyValue[0]] = keyValue[1];
                if (dataMap.count(keyValue[0]) == 0) {
                    dataMap[keyValue[0]] = new Util::ScrollingBuffer();
                }
                if (showMap.count(keyValue[0]) == 0) {
                    showMap[keyValue[0]] = new bool(false);
                }
            }
            std::cout<<telemetryMap["generator voltage"]<<std::endl;
        }

        if (savingFile) {
            auto now = std::chrono::system_clock::now();
            auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
            auto value = ms.time_since_epoch().count();
            file << value << ",";
            for (const auto &csvHeader: csvHeaders) {
                file << telemetryMap[csvHeader] << ",";
                //std::cout << telemetryMap[csvHeader] << ",";
            }
            file << std::endl;
            //std::cout << std::endl;
        }

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
//    for (int i = 0; i < keys.size(); i++) {
//        ImGui::Text("%s", keys[i].c_str());
//        ImGui::NextColumn();
//        ImGui::Text("%s", values[i].c_str());
//        ImGui::NextColumn();
//        ImGui::Separator();
//    }

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

    Setting::telemetryMutex.lock();
    Setting::telemetryStr = std::string("");
    Setting::telemetryMutex.unlock();

    dataMap.clear();
    showMap.clear();
    savingFile = false;
    csvHeaders.clear();
    telemetryMap.clear();
    file.close();
}

TelemetryPanel::~TelemetryPanel() {
}


void TelemetryPanel::graphData() {
    if (!initalized) {
        return;
    }
    for (const auto &pair: telemetryMap) {
        ImGui::Checkbox(pair.first.c_str(), showMap[pair.first]);
        ImGui::SameLine();
    }
//    for (int i = 0; i < keys.size(); i++) {
//        ImGui::Checkbox(keys[i].c_str(), showMap[keys[i]]);
//        ImGui::SameLine();
//    }
    ImGui::SliderFloat("History", &history, 1, 60, "%.1f s");
    ImGui::SameLine();
    ImGui::Checkbox("Auto Scale", &autoScale);

    static float t = 0;

    if (!paused) {
        t += ImGui::GetIO().DeltaTime;
        for (const auto &pair: telemetryMap) {
            if (*showMap[pair.first]) {
                dataMap[pair.first]->AddPoint(t, std::stof(pair.second));
            }
        }
//        for (int i = 0; i < keys.size(); i++) {
//            if (*showMap[keys[i]]) {
//                dataMap[keys[i]]->AddPoint(t, std::stof(values[i]));
//            }
//        }
    }
    if (autoScale) {
        ImPlot::SetNextAxesToFit();
    }

    if (ImPlot::BeginPlot("##Digital", ImVec2(-1, -1), ImPlotAxisFlags_AutoFit)) {
        ImPlot::SetupAxes(nullptr, nullptr);
        ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
        for (const auto &pair: telemetryMap) {
            if (*showMap[pair.first]) {
                ImPlot::PlotLine(pair.first.c_str(), &dataMap[pair.first]->Data[0].x, &dataMap[pair.first]->Data[0].y,
                                 dataMap[pair.first]->Data.size(), 0,
                                 dataMap[pair.first]->Offset, 2 * sizeof(float));
            }
        }
//        for (int i = 0; i < keys.size(); ++i) {
//            if (*showMap[keys[i]]) {
//                ImPlot::PlotLine(keys[i].c_str(), &dataMap[keys[i]]->Data[0].x, &dataMap[keys[i]]->Data[0].y,
//                                 dataMap[keys[i]]->Data.size(), 0,
//                                 dataMap[keys[i]]->Offset, 2 * sizeof(float));
//
//            }
//        }
        ImPlot::EndPlot();
    }
}
