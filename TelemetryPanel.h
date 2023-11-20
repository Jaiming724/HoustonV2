#pragma once

#include <string>
#include "Component.h"
#include "Util.h"


class TelemetryPanel : public Component {
private:
    std::vector<std::string> keys;
    std::vector<std::string> values;
    bool paused = false;
    Util::ScrollingBuffer *dataAnalog = nullptr;
    bool *showAnalog = nullptr;
    int keySize = -1;

public:

    TelemetryPanel(const char *name) : Component(name) {
    }

    ~TelemetryPanel() override;

    void start() override;

    void render() override;

    void stop() override;
    void Demo_DigitalPlots();
};