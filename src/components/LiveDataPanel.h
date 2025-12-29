
#pragma once

#include "../pch.h"
#include "Component.h"

class LiveDataPanel : public Component {
private:
    std::map<uint16_t, LiveDataPacket_t> liveDataMap;
    std::map<uint16_t, int32_t *> intMap;
    std::map<uint16_t, float *> floatMap;

    void sendModifyPacket(uint8_t packetID, uint16_t valueType);
    void refreshPackets();

public:
    LiveDataPanel(const char *name, Dispatcher *dispatcher) : Component(name, dispatcher) {};

    ~LiveDataPanel() override;

    void start() override;

    void render() override;

    void stop() override;
};


