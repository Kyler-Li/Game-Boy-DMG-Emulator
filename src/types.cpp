#include "types.h"

namespace gbdmge {
// CPU Register
Reg::Reg() : low(0), high(0) {}

word Reg::get() {
  return static_cast<word>(low) | static_cast<word>(high) << 8;
}

void Reg::set(const word value) {
  high = static_cast<byte>(value >> 8);
  low = static_cast<byte>(value);
}

void Reg::clear() {
  high = 0;
  low = 0;
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

// Memory Register
MemoryReg::MemoryReg(byte& value) : data(value) {}

byte MemoryReg::get() {
  return data;
}

void MemoryReg::set(const byte value) {
  data = value;
}

void MemoryReg::clear() {
  data = 0;
}

void MemoryReg::inc() {
  data++;
}

void MemoryReg::dec() {
  data--;
}
}  // namespace gbdmge
