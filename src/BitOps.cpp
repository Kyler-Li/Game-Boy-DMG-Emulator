#include "BitOps.h"

namespace gbdmge {

constexpr void setBit(byte& value, const byte pos, const bool set) {
  const byte mask = static_cast<byte>(1) << pos;
  if (set) {
    value |= mask;
  } else {
    value &= ~mask;
  }
}

constexpr bool isBitSet(const byte& value, const byte pos) {
  return (value & static_cast<byte>(1) << pos) != 0;
}

constexpr word makeWord(const byte low, const byte high) {
  return static_cast<word>(low) | static_cast<word>(high) << 8;
}

constexpr byte lowByte(const word word) {
  return static_cast<byte>(word);
}

constexpr byte highByte(const word word) {
  return static_cast<byte>(word >> 8);
}

}  // namespace gbdmge