#pragma once

#include "../pch.h"
#include "Component.h"
#include "../Setting.h"
#include "../Util.h"
#include "../services/Dispatcher.h"

class AlertPanel : public Component {
private:

    Dispatcher *dispatcher;
public:
    std::vector<std::string> alerts;
    AlertPanel(const char *name, Dispatcher *dispatcher) : Component(name) {
        this->dispatcher = dispatcher;
    };

    ~AlertPanel() override;

    void start() override;

    void render() override;

    void stop() override;
};