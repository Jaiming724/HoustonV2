#pragma once

#include <fstream>
#include "../pch.h"

#include "Component.h"
#include "../Util.h"
#include "../Setting.h"
#include "../services/Dispatcher.h"

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
    Dispatcher *dispatcher;
    int timer = 0;
    nlohmann::json jsonParser;
    std::unordered_map<uint16_t, nlohmann::json> jsonMap;

    NFD::Guard nfdGuard;
    NFD::UniquePath outPath;
    nfdfilteritem_t filterItem[1] = {{"jsonFile", "json"}};
public:

    TelemetryPanel(const char *name, Dispatcher *dispatcher) : Component(name) {
        this->dispatcher = dispatcher;

    }

    ~TelemetryPanel() override;

    void start() override;

    void render() override;

    void stop() override;

    void parseCanPacket(const nlohmann::json &dbc, const uint8_t *packet);

    void graphData();
};