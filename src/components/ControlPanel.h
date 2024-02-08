#pragma once
#include "../pch.h"
#include "Component.h"
#include <string>
#include <iostream>
#include <cstdio> // Include the header for sprintf

#include "../SerialHelper.h"

// this can be defined in a separate file
#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif   

class ControlPanel : public Component {
private:
    SerialHelper *reader;
    std::vector<Component *> *pVector;
public:

    ControlPanel(const char *name, SerialHelper *reader, std::vector<Component *> *pVector) : Component(name) {
        this->reader = reader;
        this->pVector = pVector;
    }

    ~ControlPanel() override;

    void start() override;

    void render() override;

    void stop() override;

    std::vector<std::string> ports();
};