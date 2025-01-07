
#pragma once

#include "../pch.h"
#include "Component.h"
#include "../SerialHelper.h"
#include "../services/WebSocketProducer.h"
#include "../services/Dispatcher.h"

class LiveDataPanel : public Component {
private:
    WebSocketProducer *webSocketProducer;
    Dispatcher *dispatcher;
    std::map<std::string, int *> intMap;
    std::map<std::string, float *> floatMap;
    std::map<std::string, bool *> boolMap;
    std::map<std::string, std::string> dataMap;
    bool test = true;

public:
    LiveDataPanel(const char *name, WebSocketProducer *webSocketProducer, Dispatcher *dispatcher)
            : Component(name) {
        this->webSocketProducer = webSocketProducer;
        this->dispatcher = dispatcher;
    }

    ~LiveDataPanel() override;

    void start() override;

    void render() override;

    void stop() override;
};


