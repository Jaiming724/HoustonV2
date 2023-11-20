#pragma once

#include <string>
#include "Component.h"


class TelemetryPanel : public Component {
private:
    std::string str;
public:

    TelemetryPanel(const char *name) : Component(name) {
    }

    ~TelemetryPanel() override;

    void start() override;

    void render() override;

    void stop() override;
};