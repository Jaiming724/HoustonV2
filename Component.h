#pragma once

#include <string>
#include "imgui.h"

class Component {
private:
    const char* name;

public:
    Component(const char* name) {
        this->name = name;
    }

    virtual ~Component() {
    }

    virtual void start() {};

    virtual void render() {};

    virtual void stop() {};

};