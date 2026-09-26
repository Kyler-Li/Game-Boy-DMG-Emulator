#pragma once

#include <cstdint>

using byte = std::uint8_t;
using word = std::uint16_t;


class RegPair{
    byte upper;
    byte lower;
public:
    RegPair() : RegPair(0, 0) {}
    RegPair(byte upper, byte lower)
        : upper(upper), lower(lower) {}

    void set(byte upper, byte lower);
    void set(word word);
    word get();
    byte getUpper();
    byte getLower();
};