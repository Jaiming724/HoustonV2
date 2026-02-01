#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#else
#include <filesystem>
#endif
namespace Util {
    inline std::vector<std::string> getAvailablePorts() {
        std::vector<std::string> ports;
#ifdef _WIN32
        char portPath[20];
        for (int i = 1; i <= 256; ++i) {
            std::sprintf(portPath, "\\\\.\\COM%d", i);
            HANDLE hSerial = CreateFile(portPath, GENERIC_READ | GENERIC_WRITE, 0, 0,
                                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
            if (hSerial != INVALID_HANDLE_VALUE) {
                ports.push_back("COM" + std::to_string(i));
                CloseHandle(hSerial);
            }
        }
#else
        const std::string prefixes[] = {"ttyUSB", "ttyACM", "tty.", "cu."};
        if (std::filesystem::exists("/dev")) {
            for (const auto &entry: std::filesystem::directory_iterator("/dev")) {
                std::string filename = entry.path().filename().string();
                for (const auto &prefix: prefixes) {
                    if (filename.compare(0, prefix.length(), prefix) == 0) {
                        ports.push_back(entry.path().string());
                        break;
                    }
                }
            }
        }
#endif
        return ports;
    }

    struct ScrollingBuffer {
        int MaxSize;
        int Offset;
        ImVector<ImVec2> Data;

        ScrollingBuffer(int max_size = 5000) {
            MaxSize = max_size;
            Offset = 0;
            Data.reserve(MaxSize);
        }

        void AddPoint(float x, float y) {
            if (Data.size() < MaxSize)
                Data.push_back(ImVec2(x, y));
            else {
                Data[Offset] = ImVec2(x, y);
                Offset = (Offset + 1) % MaxSize;
            }
        }

        void Erase() {
            if (Data.size() > 0) {
                Data.shrink(0);
                Offset = 0;
            }
        }
    };
} // namespace Util
