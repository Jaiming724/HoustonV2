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
    std::vector<Util::ScrollingBuffer *> data;
    std::vector<bool *> showAnalog;
    bool initalized = false;
    float history = 10.0f;
    bool autoScale = true;
    int keysize = -1;
public:

    TelemetryPanel(const char *name) : Component(name) {
    }

    ~TelemetryPanel() override;

    void start() override;

    void render() override;

    void stop() override;

    void graphData();
};