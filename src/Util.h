#pragma once

#include <string>
#include <vector>
#include <algorithm>
namespace Util {

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