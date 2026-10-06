#include "bitOps.h"

namespace gbdmge {

void setBitByte(byte& value, const byte pos, const bool set) {
  const byte mask = static_cast<byte>(1) << pos;
  if (set) {
    value |= mask;
  } else {
    value &= ~mask;
  }
}

bool bitIsSetByte(const byte& value, const byte pos) {
  return (value & static_cast<byte>(1) << pos) != 0;
}

word makeWordLowHighBytes(const byte low, const byte high) {
  return static_cast<word>(low) | static_cast<word>(high) << 8;
}

byte getLowByteFromWord(const word word) {
  return static_cast<byte>(word);
}

byte getHighByteFromWord(const word word) {
  return static_cast<byte>(word >> 8);
}

}  // namespace gbdmge