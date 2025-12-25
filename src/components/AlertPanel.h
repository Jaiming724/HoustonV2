#pragma once

#include "../pch.h"
#include "Component.h"
#include "../Setting.h"
#include "../Util.h"

class AlertPanel : public Component {
private:
    std::vector<std::string> alerts;
public:
    AlertPanel(const char *name, Dispatcher *dispatcher) : Component(name, dispatcher) {};

    ~AlertPanel() override;

    void start() override;

    void render() override;

    void stop() override;
};