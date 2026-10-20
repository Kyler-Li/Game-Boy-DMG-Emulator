#ifndef GBDMGE_JOYPAD_H
#define GBDMGE_JOYPAD_H

#include "Types.h"

namespace gbdmge {

class Joypad {
 public:
  Joypad() = default;
  byte read(word address);
  void write(word address, byte value);
  void reset();  // Intialize to post boot rom state
 private:
  byte p1;
};

}  // namespace gbdmge

#endif