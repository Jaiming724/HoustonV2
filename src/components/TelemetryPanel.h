#pragma once

#include <fstream>
#include "../pch.h"

#include "Component.h"
#include "../Util.h"
#include "../Setting.h"

class TelemetryPanel : public Component {
private:
//    std::vector<std::string> keys;
//    std::vector<std::string> values;
    std::map<std::string, std::string> telemetryMap;
    bool paused = false;
    bool initalized = false;
    float history = 10.0f;
    bool autoScale = true;
    std::map<std::string, Util::ScrollingBuffer *> dataMap;
    std::map<std::string, bool *> showMap;
    char csvFileBuffer[256] = "data.csv"; // Buffer to store input text
    bool savingFile = false;
    std::vector<std::string> csvHeaders;
    std::ofstream file;

    int timer = 0;

public:

    TelemetryPanel(const char *name) : Component(name) {
    }

    ~TelemetryPanel() override;

    void start() override;

    void render() override;

    void stop() override;

    void graphData();
};