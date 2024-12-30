#pragma once

#include "../pch.h"

class DataProducer {

public:
    const char *name;
    bool status = false;
    DataProducer(const char *name) {
        this->name = name;
    }

    virtual ~DataProducer() {
    }

    virtual void init() {};

    virtual void start() {};

    virtual void fetch() {};

    virtual void stop() {};

};