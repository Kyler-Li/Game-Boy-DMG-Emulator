#pragma once

#include <cstdint>

using byte = std::uint8_t;
using word = std::uint16_t;
using qword = std::uint64_t;

class Reg {
public:
    byte low;
    byte high;

    Reg();
    explicit operator word() const;
    Reg& operator=(word value);
    Reg& operator++();
    Reg operator++(int);
    Reg& operator--();
    Reg operator--(int);
    void setBit(byte& value, byte pos, bool set);
};