
#pragma once
#include "../pch.h"
#include "Component.h"
#include "../SerialHelper.h"
#include "../services/WebSocketProducer.h"
class LiveDataPanel : public Component {
private:
    WebSocketProducer *webSocketProducer;
    std::map<std::string, int*> intMap;
    std::map<std::string, float*> floatMap;
    std::map<std::string, bool*> boolMap;
    std::map<std::string,std::string> dataMap;

public:
    LiveDataPanel(const char *name, WebSocketProducer *webSocketProducer);

    ~LiveDataPanel() override;

    void start() override;

    void render() override;

    void stop() override;
};


