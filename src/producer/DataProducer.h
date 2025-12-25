#pragma once

#include "../Dispatcher.h"

class DataProducer {

public:
    const char *name;
    bool status = false;

    DataProducer(const char *str) : name(str) {

    }

    virtual ~DataProducer() = default;

    virtual void init() {};

    virtual bool start() {
        return false;
    };

    virtual void produce(Dispatcher &dispatcher) {};

    virtual void stop() {};

};