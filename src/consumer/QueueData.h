#pragma once

#include <queue>
#include "DataConsumer.h"

class QueueData : public DataConsumer {

public:
    std::queue<std::vector<uint8_t>> queue;

    void consume(std::vector<uint8_t> data) override;
};