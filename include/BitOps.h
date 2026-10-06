#ifndef GBDMGE_BIT_OPS_H
#define GBDMGE_BIT_OPS_H

#include "Types.h"

namespace gbdmge {

constexpr void setBit(byte& value, byte pos, bool set);
constexpr bool bitIsSet(const byte& value, byte pos);
constexpr word makeWord(byte high, byte low);
constexpr byte getHighByte(word word);
constexpr byte getLowByte(word word);
}  // namespace gbdmge

#endif
