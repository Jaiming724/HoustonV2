#pragma once
#include "../pch.h"
#include "Component.h"
#include "../SerialHelper.h"

class ControlPanel : public Component {
private:
    SerialHelper *reader;
    std::vector<Component *> *pVector;
public:

    ControlPanel(const char *name, SerialHelper *reader, std::vector<Component *> *pVector) : Component(name) {
        this->reader = reader;
        this->pVector = pVector;
    }

    ~ControlPanel() override;

    void start() override;

    void render() override;

    void stop() override;
};