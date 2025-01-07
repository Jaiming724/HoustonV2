#include "QueueData.h"

void QueueData::consume(std::vector<uint8_t> &data) {
    std::string result(data.begin() + 1, data.end());
    queue.push(result);
}