#pragma once

#include "../Dispatcher.h"

class DataProducer {

public:
    const char *name;
    bool status = false;

    explicit DataProducer(const char *str) : name(str) {

    }

    virtual ~DataProducer() = default;

    virtual void init()=0;

    virtual bool start()=0;
    virtual void produce(Dispatcher*dispatcher)=0;

    virtual void stop()=0;

};


