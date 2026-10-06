#ifndef GBDMGE_BIT_OPS_H
#define GBDMGE_BIT_OPS_H

#include "types.h"

namespace gbdmge {

void setBitByte(byte& value, byte pos, bool set);
bool bitIsSetByte(const byte& value, byte pos);
word makeWordLowHighBytes(byte low, byte high);
byte getHighByteFromWord(word word);
byte getLowByteFromWord(word word);

}  // namespace gbdmge

#endif
