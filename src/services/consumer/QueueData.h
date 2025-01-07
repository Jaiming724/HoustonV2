#pragma once

#include "DataConsumer.h"
#include <queue>

class QueueData : public DataConsumer {

public:
    void consume(std::vector<uint8_t> &data) override;
    std::queue<std::string> queue;
};