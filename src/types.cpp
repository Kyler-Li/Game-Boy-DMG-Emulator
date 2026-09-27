#include "types.h"

Reg::Reg() : low(0), high(0) {}

Reg::operator word() const {
    return static_cast<word>(low) | static_cast<word>(high) << 8;
}

Reg &Reg::operator=(const word value) {
    low = static_cast<byte>(value);
    high = static_cast<byte>(value >> 8);
    return *this;
}

Reg &Reg::operator++() {
    *this = static_cast<word>(*this) + 1;
    return *this;
}

Reg Reg::operator++(int) {
    const Reg old {*this};
    ++*this;
    return old;
}

Reg &Reg::operator--() {
    *this = static_cast<word>(*this) - 1;
    return *this;
}

Reg Reg::operator--(int){
    const Reg old {*this};
    --*this;
    return old;
}

void Reg::setBit(byte& value, const byte pos, const bool set) {
    const byte mask = 1 << pos;
    if (set) {
        value |= mask;
    }else {
        value &= ~mask;
    }
}

