#pragma once

#include "../pch.h"
class Component {

public:
    Component(const char* name) {
        this->name = name;
    }
    const char* name;

    virtual ~Component() {
    }

    virtual void start() {};

    virtual void render() {};

    virtual void stop() {};


};