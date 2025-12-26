#pragma once

#include <queue>
#include <optional>
#include "DataConsumer.h"

template<typename T>
class TypedConsumer : public DataConsumer {
protected:
    std::queue<T> queue;

    virtual T decode(std::vector<uint8_t> &data) = 0;

public:
    void consume(std::vector<uint8_t> data) override {
        T decodedVal = decode(data);

        queue.push(decodedVal);
    }

    std::optional<T> pop() {
        if (queue.empty()) {
            return std::nullopt;
        }
        T val = queue.front();
        queue.pop();
        return val;
    }

    bool isEmpty() {
        return queue.empty();
    }
};