#pragma once

#include <string>
#include <mutex>
#include <queue>

namespace Setting {
    inline bool isEnable = false;
    inline std::string portName;
    inline std::string telemetryStr;
    inline std::queue<std::string> alertQueue;
    inline std::string modifyStr;
    inline std::mutex telemetryMutex;
    inline std::mutex alertMutex;
    inline std::mutex modifyMutex;
    inline std::mutex isEnableMutex;
}