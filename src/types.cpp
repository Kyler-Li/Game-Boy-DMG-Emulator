#include "types.h"

void RegPair::set(byte upper, byte lower) {
    this->upper = upper;
    this->lower = lower;
}

void RegPair::set(word value) {
    upper = (value >> 8) & 0xFF;
    lower = value & 0xFF;
}

word RegPair::get() {
    return (static_cast<word>(upper) << 8) | static_cast<word>(lower);
}

byte RegPair::getUpper() {
    return upper;
}

byte RegPair::getLower() {
    return lower;
}
