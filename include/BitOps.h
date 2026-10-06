#ifndef GBDMGE_BIT_OPS_H
#define GBDMGE_BIT_OPS_H

#include "Types.h"

namespace gbdmge {

constexpr void setBitByte(byte& value, byte pos, bool set);
constexpr bool bitIsSetByte(const byte& value, byte pos);
constexpr word makeWordHighLowBytes(byte high, byte low);
constexpr byte highByteFromWord(word word);
constexpr byte lowByteFromWord(word word);

}  // namespace gbdmge

#endif
