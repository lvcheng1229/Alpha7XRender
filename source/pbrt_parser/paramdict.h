// pbrt is Copyright(c) 1998-2020 Matt Pharr, Wenzel Jakob, and Greg Humphreys.
// The pbrt source code is licensed under the Apache License, Version 2.0.
// SPDX: Apache-2.0

#ifndef PBRT_PARAMDICT_H
#define PBRT_PARAMDICT_H

#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <array>

namespace pbrt {
    // ParsedParameter Definition
    class ParsedParameter {
    public:
        // ParsedParameter Public Methods
        ParsedParameter() {}

        void AddFloat(float v);
        void AddInt(int i);
        void AddString(std::string_view str);
        void AddBool(bool v);

        std::string ToString() const;

        // ParsedParameter Public Members
        std::string type, name;
        std::vector<float> floats;
        std::vector<int> ints;
        std::vector<std::string> strings;
        std::vector<uint8_t> bools;
        mutable bool lookedUp = false;
        bool mayBeUnused = false;
    };

    // ParsedParameterVector Definition
    using ParsedParameterVector = std::vector<ParsedParameter*>;
};



#endif