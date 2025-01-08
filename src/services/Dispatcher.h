#pragma once

#include <unordered_map>
#include "consumer/DataConsumer.h"
#include <string>
#include <optional>
#include <iostream>
#include <memory>
#include <utility>

class Dispatcher {
private:
    std::unordered_map<std::string, std::shared_ptr<DataConsumer>> handlerMap;
public:
    void registerHandler(const std::string &key, std::shared_ptr<DataConsumer> handler) {
        //TODO check if std::move is smart here
        handlerMap[key] = std::move(handler);
    }

    void dispatchData(std::vector<uint8_t> &data) {
        if (!data.empty() && data[0] == 0xe0) {
            auto it = handlerMap.find("AlertConsumer");
            if (it != handlerMap.end()) {
                std::cout << "found it" << std::endl;
                it->second->consume(data);
            }
        } else if (!data.empty() && data[0] == 0x19){
            auto it = handlerMap.find("LiveDataConsumer");
            if (it != handlerMap.end()) {
                std::cout << "found live data consumer" << std::endl;
                it->second->consume(data);
            }
        }
    }

    std::shared_ptr<DataConsumer> getHandler(const std::string &key) {
        auto it = handlerMap.find(key);
        if (it != handlerMap.end()) {
            return it->second;
        }
        return nullptr;
    }

};