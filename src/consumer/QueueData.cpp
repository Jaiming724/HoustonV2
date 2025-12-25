
#include "QueueData.h"

void QueueData::consume(std::vector<uint8_t> data) {
    queue.push(std::move(data));
}

