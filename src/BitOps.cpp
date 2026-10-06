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

constexpr bool bitIsSet(const byte& value, const byte pos) {
  return (value & static_cast<byte>(1) << pos) != 0;
}

constexpr word makeWord(const byte high, const byte low) {
  return static_cast<word>(high) << 8 | static_cast<word>(low);
}

constexpr byte getLowByte(const word word) {
  return static_cast<byte>(word);
}

constexpr byte getHighByte(const word word) {
  return static_cast<byte>(word >> 8);
}

}  // namespace gbdmge