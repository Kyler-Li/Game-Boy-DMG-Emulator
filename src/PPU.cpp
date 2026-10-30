#include "PPU.h"

#include <assert.h>

namespace gbdmge {
PPU::PPU()
    : state{PPUState::OAM_SCAN},
      dots{0},
      lcdc{},
      stat{},
      ly{},
      lyc{},
      scx{},
      scy{},
      wx{},
      wy{},
      bgp{},
      obp0{},
      obp1{},
      vram{} {}

byte PPU::read(const word address) {
  return 0xFF;
}

void PPU::write(word address, byte value) {
  // TODO: Implement
}

void PPU::reset() {
  // TODO: Implement
}

void PPU::tick(qword cycles) {
  switch (state) {
    case PPUState::OAM_SCAN:
      break;
    case PPUState::DRAW_PIXELS:
      break;
    case PPUState::H_BLANK:
      hblank();
      break;
    case PPUState::V_BLANK:
      vblank();
      break;
    default:
      assert(false && "Impossible PPU state reached.");
  }
  dots++;
}

void PPU::hblank() {
  if (dots != PPU_DOTS_PER_SCANLINE) {
    return;
  }

  // Drop down to next scanline
  ++ly;

  if (ly < PPU_SCANLINE_VBLANK_START) {
    setState(PPUState::OAM_SCAN);
  } else {
    setState(PPUState::V_BLANK);
  }
}

void PPU::vblank() {
  if (dots != PPU_DOTS_PER_SCANLINE) {
    return;
  }

  if (ly < PPU_SCANLINE_VBLANK_END) {
    ++ly;
  } else {
    ly = 0;
    setState(PPUState::OAM_SCAN);
  }

  dots = 0;
}

// Sets the state of the PPU, and the lower 2 bits of the STAT register
void PPU::setState(PPUState s) {
  const byte statBits = static_cast<byte>(s) & MASK_2_LOWER_BYTE;
  stat &= ~MASK_2_LOWER_BYTE;
  stat |= statBits;
  state = s;
}

}  // namespace gbdmge
