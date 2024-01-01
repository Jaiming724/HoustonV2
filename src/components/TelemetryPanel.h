#pragma once

#include "../pch.h"

#include "Component.h"
#include "../Util.h"
#include "../Setting.h"

class TelemetryPanel : public Component {
private:
    std::vector<std::string> keys;
    std::vector<std::string> values;
    bool paused = false;
    bool initalized = false;
    float history = 10.0f;
    bool autoScale = true;
    std::map<std::string, Util::ScrollingBuffer *> dataMap;
    std::map<std::string, bool*> showMap;

public:

    TelemetryPanel(const char *name) : Component(name) {
    }

    ~TelemetryPanel() override;

    void start() override;

    void render() override;

    void stop() override;

    void graphData();
};