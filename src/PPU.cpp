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

  // The number of T-cycles needed to draw one scanline
  constexpr qword numScanlineCycles = 456;

  // The total number of scanlines
  constexpr qword numScanlines = 154;

  if (cycleCounter >= numScanlineCycles) {
    cycleCounter -= numScanlineCycles;
    ++ly;

    if (ly == numScanlines) {
      ly = 0;
    }
  }
}

}  // namespace gbdmge
