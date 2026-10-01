#include "types.h"

namespace gbdmge {
// CPU Register
Register::Register() : low(0), high(0) {}

word Register::get() {
  return static_cast<word>(low) | static_cast<word>(high) << 8;
}

void Register::set(const word value) {
  high = static_cast<byte>(value >> 8);
  low = static_cast<byte>(value);
}

void Register::clear() {
  high = 0;
  low = 0;
}

void Register::inc() {
  set(get() + 1);
}

void Register::dec() {
  set(get() - 1);
}

byte& Register::getLow() {
  return low;
}

byte& Register::getHigh() {
  return high;
}

void Register::setLow(const byte value) {
  low = value;
}

void Register::setHigh(const byte value) {
  high = value;
}

// Memory Register
MemoryRegister::MemoryRegister(byte& value) : data(value) {}

byte MemoryRegister::get() {
  return data;
}

void MemoryRegister::set(const byte value) {
  data = value;
}

void MemoryRegister::clear() {
  data = 0;
}

void MemoryRegister::inc() {
  data++;
}

void MemoryRegister::dec() {
  data--;
}
}  // namespace gbdmge
