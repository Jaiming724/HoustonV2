#pragma once

#include "../pch.h"
#include "Component.h"
#include "../Dispatcher.h"
#include "../producer/DataProducer.h"

class ControlPanel : public Component {
private:
    std::vector<Component *> *pVector;
    bool isEnable = false;
    DataProducer *dataProducer = nullptr;
    std::vector<std::string> availablePorts;
    int selectedPortIndex = 0;

public:
    ControlPanel(const char *name, Dispatcher *dispatcher, std::vector<Component *> *pVector) : Component(name, dispatcher) {
        this->pVector = pVector;
    }

    ~ControlPanel() override;

    void start() override;

    void render() override;

    void stop() override;
};
