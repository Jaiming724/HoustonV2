#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include <iostream>
#include <cstdio> // Include the header for sprintf



namespace Util {

#ifdef IS_WINDOWS
    inline std::string ports(std::vector<std::string> *portsList) {
        std::vector<std::string> stringVector;
        std::string listString = "";
        char port[20];
        HANDLE hSerial;

        for (int i = 1; i <= 256; i++)
        {
            int com_check_flag = 0;
            std::sprintf(port, "\\\\.\\com%d", i);

            hSerial = CreateFile(port, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
            if (hSerial == INVALID_HANDLE_VALUE)
            {

                com_check_flag = 1;

            }
            else {

                std::string port = "COM" + std::to_string(i);
                std::cout << "Detected port: " << port << std::endl;

                listString += port + '\0';
                stringVector.push_back(port);
                CloseHandle(hSerial);

            }
        }
        listString += '\0';
        *portsList = stringVector;

        return listString;
    }
#endif

    inline auto splitString(std::string in, char sep) {
        std::vector<std::string> r;
        r.reserve(std::count(in.begin(), in.end(), sep) + 1); // optional
        for (auto p = in.begin();; ++p) {
            auto q = p;
            p = std::find(p, in.end(), sep);
            r.emplace_back(q, p);
            if (p == in.end())
                return r;
        }
    }

    inline uint16_t toLittleEndian16(uint16_t value) {
        return ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
    }

// Function to convert a 32-bit integer to little-endian
    inline uint32_t toLittleEndian32(uint32_t value) {
        return ((value & 0xFF) << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) |
               ((value & 0xFF000000) >> 24);
    }

    struct ModifyPacket {
        uint8_t packet_id = 13;
        uint16_t packet_length = 0x3;
        char string_data[3] = {0};
        union {
            float float_data;
            uint32_t int_data;
        };
        uint32_t checksum;
    };


    inline uint32_t crc32(const char *s, size_t n) {
        uint32_t crc = 0xFFFFFFFF;

        for (size_t i = 0; i < n; i++) {
            char ch = s[i];
            for (size_t j = 0; j < 8; j++) {
                uint32_t b = (ch ^ crc) & 1;
                crc >>= 1;
                if (b) crc = crc ^ 0xEDB88320;
                ch >>= 1;
            }
        }

        return ~crc;
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
}