#include <bitset>
#include "TelemetryPanel.h"
#include "../services/consumer/QueueData.h"

void TelemetryPanel::start() {
    telemetryMap.clear();
}


uint64_t extractBits(const uint8_t *data, int startBit, int bitLength, const std::string &endianess) {
    uint64_t raw = 0;

    // Flatten the 64-bit CAN payload to a bit string
    std::bitset<64> bitset;
    for (int i = 0; i < 8; ++i) {
        std::bitset<8> byteBits(data[i]);
        for (int b = 0; b < 8; ++b) {
            bitset[(7 - i) * 8 + b] = byteBits[b];  // big-endian layout
        }
    }

    if (endianess == "intel") {
        for (int i = 0; i < bitLength; ++i) {
            int byteIndex = (startBit + i) / 8;
            int bitInByte = (startBit + i) % 8;
            bool bit = (data[byteIndex] >> bitInByte) & 0x1;
            raw |= (uint64_t(bit) << i);
        }
    } else {
        // Motorola: MSB-first within byte, big-endian bytes
        // Reverse Motorola logic is tricky: real DBC parsing tools handle this
        int byte = startBit / 8;
        int bitInByte = startBit % 8;
        int bitIndex = (7 - byte) * 8 + (7 - bitInByte);  // flip byte and bit

        for (int i = 0; i < bitLength; ++i) {
            raw <<= 1;
            raw |= bitset[bitIndex - i];
        }
    }

    return raw;
}

double applyScaling(double raw, double scale, double offset) {
    return raw * scale + offset;
}

void TelemetryPanel::parseCanPacket(const nlohmann::json &dbc, const uint8_t *packet) {
    uint16_t canId = (packet[0] << 8) | packet[1];
    const uint8_t *data = packet + 2;
    auto it = jsonMap.find(canId);

    if (it != jsonMap.end()) {
        const nlohmann::json &msg = it->second;

        //std::cout << "Matched Message: " << msg["name"] << "\n";

        for (const auto &sig: msg["signals"]) {
            std::string name = sig["name"];
            int startbit = sig["startbit"];
            int bitlength = sig["bitlength"];
            std::string endianess = sig["endianess"];
            double scale = sig["scaling"];
            double offset = sig["offset"];
            bool isSigned = sig["signed"];
            std::string units = sig["units"];

            uint64_t raw = extractBits(data, startbit, bitlength, endianess);

            // Sign extension if needed
            if (isSigned && bitlength < 64) {
                int64_t signedRaw = raw;
                int64_t signBit = 1LL << (bitlength - 1);
                if (signedRaw & signBit) {
                    signedRaw |= ~((1LL << bitlength) - 1);
                }
                double value = applyScaling(static_cast<double>(signedRaw), scale, offset);
                if (canId == 0x500) {
                    std::cout << name << ": " << value << " " << units << "\n";

                }
                std::string key = std::to_string(value) + " " + units;

                telemetryMap[name] = key;

            } else {
                double value = applyScaling(static_cast<double>(raw), scale, offset);
                if (canId == 0x500) {
                    std::cout << name << ": " << value << " " << units << "\n";
                }
                std::string key = std::to_string(value) + " " + units;

                telemetryMap[name] = key;
            }


            return;
        }
    }

    std::cout << "CAN ID " << canId << " not found in JSON.\n";
}

void TelemetryPanel::render() {
    timer += ImGui::GetIO().DeltaTime * 1000; // Convert deltaTime to milliseconds
    QueueData *queueData = (QueueData *) dispatcher->getHandler(std::string("TelemetryConsumer")).get();
    while (!queueData->queue.empty()) {
        auto arr = queueData->queue.front();
        uint16_t id = ((uint8_t) arr[0] << 8) | (uint8_t) arr[1];
        parseCanPacket(jsonParser, arr.data());


//        if (id == 0x500) {
//            std::cout << "heartbeat" << std::endl;
//        }
        queueData->queue.pop();

    }
//    if (Setting::telemetryStr.length() >= 4) {
//        timer = 0;
//        initalized = true;
//
//        telemetryMap.clear();
//        Setting::telemetryMutex.lock();
//        std::string remainingString = Setting::telemetryStr.substr(4);
//        Setting::telemetryMutex.unlock();
//        std::vector<std::string> tokens = Util::splitString(remainingString, ';');
//        for (auto &s: tokens) {
//            std::vector<std::string> keyValue = Util::splitString(s, ':');
//            if (keyValue.size() == 2) {
//                telemetryMap[keyValue[0]] = keyValue[1];
//                if (dataMap.count(keyValue[0]) == 0) {
//                    dataMap[keyValue[0]] = new Util::ScrollingBuffer();
//                }
//                if (showMap.count(keyValue[0]) == 0) {
//                    showMap[keyValue[0]] = new bool(false);
//                }
//            }
//        }
//
//        if (savingFile) {
//            auto now = std::chrono::system_clock::now();
//            auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
//            auto value = ms.time_since_epoch().count();
//            file << value << ",";
//            for (int i = 1; i < csvHeaders.size(); i++) {
//                if (telemetryMap.count(csvHeaders[i]) > 0) {
//                    file << telemetryMap[csvHeaders[i]] << ",";
//                } else {
//                    file << "-1,";
//                    continue;
//                }
//                //std::cout << telemetryMap[csvHeader] << ",";
//            }
//            file << std::endl;
//            //std::cout << std::endl;
//        }
//
//    }

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
            csvHeaders.push_back("Time");
            file << "Time,";
            for (auto it = dataMap.begin(); it != dataMap.end(); ++it) {
                csvHeaders.push_back(it->first);
                file << it->first << ",";
            }
            file << std::endl;

            savingFile = true;
        }
    }
    if (ImGui::Button("Select Json File")) {
        // Open a file dialog

        nfdresult_t result = NFD::OpenDialog(outPath, filterItem, 1);
        if (result == NFD_OKAY) {
            std::cout << "Success!" << std::endl << outPath.get() << std::endl;
            std::ifstream f(outPath.get());
            jsonParser = nlohmann::json::parse(f);
            jsonMap.clear();
            for (const auto &msg: jsonParser["messages"]) {
                uint16_t id = msg["id"];
                std::cout << "ID: " << id << std::endl;
                jsonMap[id] = msg;
            }
        } else if (result == NFD_CANCEL) {
            std::cout << "User pressed cancel." << std::endl;
        } else {
            std::cout << "Error: " << NFD::GetError() << std::endl;
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

TelemetryPanel::~TelemetryPanel() = default;


void TelemetryPanel::graphData() {
    if (!initalized) {
        return;
    }
    for (const auto &pair: telemetryMap) {
        ImGui::Checkbox(pair.first.c_str(), showMap[pair.first]);
        ImGui::SameLine();
    }
    ImGui::Checkbox("Auto Scale", &autoScale);
    ImGui::SameLine();
    ImGui::SliderFloat("History", &history, 1, 60, "%.1f s");


    static float t = 0;

    if (!paused) {
        t += ImGui::GetIO().DeltaTime;
        for (const auto &pair: telemetryMap) {
            if (*showMap[pair.first]) {
                dataMap[pair.first]->AddPoint(t, std::stof(pair.second));
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
        for (const auto &pair: telemetryMap) {
            if (*showMap[pair.first]) {
                ImPlot::PlotLine(pair.first.c_str(), &dataMap[pair.first]->Data[0].x, &dataMap[pair.first]->Data[0].y,
                                 dataMap[pair.first]->Data.size(), 0,
                                 dataMap[pair.first]->Offset, 2 * sizeof(float));
            }
        }
        ImPlot::EndPlot();
    }
}
