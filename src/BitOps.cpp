#include "BitOps.h"

namespace gbdmge {

void setBit(byte& value, byte pos, bool set) {
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

word makeWord(const byte high, const byte low) {
  return static_cast<word>(high) << 8 | static_cast<word>(low);
}

byte getLowByte(const word word) {
  return static_cast<byte>(word);
}

byte getHighByte(const word word) {
  return static_cast<byte>(word >> 8);
}

}  // namespace gbdmge