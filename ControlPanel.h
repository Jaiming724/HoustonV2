#pragma once

#include "Component.h"
#include "SerialHelper.h"

class ControlPanel : public Component {
private:
    SerialHelper *reader;

public:

    ControlPanel(const char *name, SerialHelper *reader) : Component(name) {
        this->reader = reader;
    }

    ~ControlPanel() override;

    void start() override;

    void render() override;

    void stop() override;
};