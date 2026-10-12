#ifndef GBDMGE_JOYPAD_H
#define GBDMGE_JOYPAD_H

#include "Types.h"

namespace gbdmge {

class Joypad {
 public:
  byte read(word address);
  void write(word address, byte value);

 private:
  byte p1;
};

}  // namespace gbdmge

#endif