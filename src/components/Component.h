#pragma once

#include "../pch.h"
#include "../Dispatcher.h"

class Component {
private:
    const char *name;
public:
    Dispatcher *dispatcher;

    Component(const char *name, Dispatcher *dispatcher) : name(name), dispatcher(dispatcher) {
    }

    virtual ~Component() = default;

    virtual void start() {};

    virtual void render() {};

    virtual void stop() {};


};