#include "PPU.h"

namespace gbdmge {
byte PPU::read(word address) {
  if (address == MEMORY_LY_ADDRESS) {
    return ly;
  }

  return 0xFF;
}

void PPU::write(word address, byte value) {
  // TODO: Implement
}

void PPU::reset() {
  // TODO: Implement
}

void PPU::tick(qword cycles) {
  cycleCounter += cycles;

  while (cycleCounter >= PPU_DOTS_PER_SCANLINE) {
    cycleCounter -= PPU_DOTS_PER_SCANLINE;
    ++ly;

    if (ly == PPU_SCANLINE_COUNT) {
      ly = 0;
    }
  }
}

}  // namespace gbdmge
