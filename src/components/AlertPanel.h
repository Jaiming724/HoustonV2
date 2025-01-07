#pragma once

#include "../pch.h"
#include "Component.h"
#include "../Setting.h"
#include "../Util.h"
#include "../services/Dispatcher.h"

class AlertPanel : public Component {
private:
    std::vector<std::string> alerts;
    Dispatcher *dispatcher;
public:
    AlertPanel(const char *name, Dispatcher *dispatcher) : Component(name) {
        this->dispatcher = dispatcher;
    };

    ~AlertPanel() override;

    void start() override;

    void render() override;

    void stop() override;
};