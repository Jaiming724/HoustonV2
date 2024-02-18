#pragma once

#include <string>
#include <mutex>

namespace Setting {
    inline bool isEnable = false;
    inline std::string portName;
    inline std::string telemetryStr;
    inline std::string alertStr;
    inline std::string modifyStr;
    inline std::mutex telemetryMutex;
    inline std::mutex alertMutex;
    inline std::mutex modifyMutex;
    inline std::mutex isEnableMutex;
    inline std::string portsCombo;
    inline std::vector<std::string> portsList;
}