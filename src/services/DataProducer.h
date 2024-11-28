#pragma once

#include "../pch.h"

class DataProducer {
private:
    const char *name;
public:
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