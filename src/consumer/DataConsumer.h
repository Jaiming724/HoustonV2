#pragma once
#include <vector>

class DataConsumer {
public:
    virtual void consume(std::vector<uint8_t> data) = 0;

    virtual ~DataConsumer() = default;
};