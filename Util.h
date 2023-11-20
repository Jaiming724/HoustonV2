#pragma once

#include <string>
#include <vector>

namespace Util {

    auto splitString(std::string in, char sep) {
        std::vector <std::string> r;
        r.reserve(std::count(in.begin(), in.end(), sep) + 1); // optional
        for (auto p = in.begin();; ++p) {
            auto q = p;
            p = std::find(p, in.end(), sep);
            r.emplace_back(q, p);
            if (p == in.end())
                return r;
        }
    }
}