#include "types.h"

namespace gbdmge {
    Reg::Reg() : low(0), high(0) {}

    word Reg::get() {
        return static_cast<word>(low) | static_cast<word>(high) << 8;
    }

    void Reg::set(const word value) {
        high = static_cast<byte>(value >> 8);
        low = static_cast<byte>(value);
    }

    void Reg::inc() {
        set(get() + 1);
    }

    void Reg::dec() {
        set(get() - 1);
    }

    byte& Reg::getLow() {
        return low;
    }

    byte& Reg::getHigh() {
        return high;
    }

    void Reg::setLow(const byte value) {
        low = value;
    }

    void Reg::setHigh(const byte value) {
        high = value;
    }
}
