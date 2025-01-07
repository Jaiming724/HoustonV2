#pragma once

#include <string>

class DataConsumer {
public:
    virtual void consume(std::vector<uint8_t> &data) = 0;

    virtual ~DataConsumer() = default;
};