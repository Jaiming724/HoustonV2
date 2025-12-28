
#pragma once

#include "../pch.h"
#include "Component.h"

class LiveDataPanel : public Component {
private:
    std::map<uint16_t, LiveDataPacket_t> liveDataMap;


public:
    LiveDataPanel(const char *name, Dispatcher *dispatcher) : Component(name, dispatcher) {};

    ~LiveDataPanel() override;

    void start() override;

    void render() override;

    void stop() override;
};


