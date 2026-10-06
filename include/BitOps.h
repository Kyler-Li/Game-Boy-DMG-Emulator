#ifndef GBDMGE_BIT_OPS_H
#define GBDMGE_BIT_OPS_H

#include "Types.h"

namespace gbdmge {

constexpr void setBit(byte& value, byte pos, bool set);
constexpr bool bitIsSetByte(const byte& value, byte pos);
constexpr word makeWord(byte low, byte high);
constexpr byte highByte(word word);
constexpr byte lowByte(word word);

}  // namespace gbdmge

#endif
