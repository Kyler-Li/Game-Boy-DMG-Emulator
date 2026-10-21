#include "BitOps.h"

namespace gbdmge {

void setBit(byte& value, const byte pos, const bool set) {
  const byte mask = static_cast<byte>(1) << pos;
  if (set) {
    value |= mask;
  } else {
    value &= ~mask;
  }
}

bool bitIsSet(const byte& value, const byte pos) {
  return (value & static_cast<byte>(1) << pos) != 0;
}

byte getBit(const byte& value, const byte pos) {
  return bitIsSet(value, pos) ? 1 : 0;
}

word makeWord(const byte high, const byte low) {
  return static_cast<word>(high) << 8 | static_cast<word>(low);
}

byte getLowByte(const word word) {
  return static_cast<byte>(word);
}

byte getHighByte(const word word) {
  return static_cast<byte>(word >> 8);
}

void addToPair(byte& high, byte& low, const sbyte value) {
  const word result = makeWord(high, low) + static_cast<word>(value);
  high = getHighByte(result);
  low = getLowByte(result);
}

}  // namespace gbdmge
