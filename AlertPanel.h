#pragma once

#include <vector>
#include "Component.h"
#include <iostream>

class AlertPanel : public Component {
private:
    std::vector<std::string> alerts;
public:
    AlertPanel(const char *name) : Component(name) {};

    ~AlertPanel() override;

    void start() override;

    void render() override;

    void stop() override;
};