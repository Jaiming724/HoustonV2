
#pragma once

#include <map>
#include "Component.h"
#include "SerialHelper.h"
#include "Util.h"
class LiveDataPanel : public Component {
private:
    SerialHelper *serialHelper;
    std::map<std::string, int*> intMap;
    std::map<std::string, float*> floatMap;
    std::map<std::string, bool*> boolMap;

public:
    LiveDataPanel(const char *name, SerialHelper *serialHelper);

    ~LiveDataPanel() override;

    void start() override;

    void render() override;

    void stop() override;
};


