#pragma once

#include "../pch.h"
#include "consumer/DataConsumer.h"
#include "Dispatcher.h"

class DataProducer {

public:
    const char *name;
    bool status = false;
    Dispatcher *dispatcher;

    DataProducer(const char *name, Dispatcher *dispatcher) {
        this->name = name;
        this->dispatcher = dispatcher;
    }

    virtual ~DataProducer() {
    }

    virtual void init() {};

    virtual bool start() {
        return false;
    };

    virtual void fetch() {};

    virtual void stop() {};

};