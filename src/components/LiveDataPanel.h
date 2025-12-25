
#pragma once

#include "../pch.h"
#include "Component.h"
#include "../producer/SerialProducer.h"

class LiveDataPanel : public Component {
private:
    std::map<std::string, int *> intMap;
    std::map<std::string, float *> floatMap;
    std::map<std::string, bool *> boolMap;
    std::map<std::string, std::string> dataMap;

public:
    LiveDataPanel(const char *name, Dispatcher *dispatcher) : Component(name, dispatcher) {};

//    ~LiveDataPanel() override;
//
//    void start() override;
//
//    void render() override;
//
//    void stop() override;
};


