#include "Joypad.h"
#include "Constants.h"

namespace gbdmge {
byte Joypad::read(word address) {
  // Release all buttons if neither buttons or d-pad is selected
  if ((p1 & 0x30) == 0x30) {
    p1 |= MASK_4_LOWER_BYTE;
  }

  return p1;
}

void Joypad::write(word address, byte value) {
  // Lower nibble is read-only, so we only write the upper nibble
  p1 = (value & MASK_4_UPPER_BYTE) | (p1 & MASK_4_LOWER_BYTE);
}

}  // namespace gbdmge