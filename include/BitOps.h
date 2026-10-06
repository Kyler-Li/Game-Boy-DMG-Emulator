#ifndef GBDMGE_BIT_OPS_H
#define GBDMGE_BIT_OPS_H

#include "Types.h"

namespace gbdmge {

void setBit(byte& value, byte pos, bool set);
bool bitIsSet(const byte& value, byte pos);
word makeWord(byte high, byte low);
byte getHighByte(word word);
byte getLowByte(word word);
}  // namespace gbdmge

#endif
