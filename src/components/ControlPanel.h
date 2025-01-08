#pragma once

#include "../pch.h"
#include "Component.h"
#include "../SerialHelper.h"
#include "../services/DataProducer.h"

class ControlPanel : public Component {
private:
    char addressBuf[30] = "192.168.4.1";
    char portBuf[10] = "8080";
    DataProducer *dataProducer;
    std::vector<Component *> *pVector;
    bool enable = false;
public:

    ControlPanel(const char *name, DataProducer *dataProducer, std::vector<Component *> *pVector) : Component(name) {
        this->dataProducer = dataProducer;
        this->pVector = pVector;
    }

    ~ControlPanel() override;

    void start() override;

    void render() override;

    void stop() override;
};