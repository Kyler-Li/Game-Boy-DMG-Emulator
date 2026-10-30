#include "PPU.h"
#include "bitops.h"

#include <assert.h>

namespace gbdmge {
PPU::PPU(Bus& bus)
    : bus{bus},
      state{PPUState::OAM_SCAN},
      dots{0},
      oamScanIndex{0},
      lcdc{0},
      stat{0},
      ly{0},
      lyc{0},
      scx{0},
      scy{0},
      wx{0},
      wy{0},
      bgp{0},
      obp0{0},
      obp1{0},
      vram{},
      oam{},
      objectAttributes{} {}

byte PPU::read(const word address) {
  return 0xFF;
}

void PPU::write(word address, byte value) {
  // TODO: Implement
}

void PPU::reset() {
  // TODO: Implement
  // Intialize PPU to state post boot rom
}

void PPU::tick(qword cycles) {
  switch (state) {
    case PPUState::OAM_SCAN:
      oamScan();
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

void PPU::oamScan() {
  // Process two OAM entries every 4 dots
  if (dots % 4 == 0) {
    for (byte i = 0; i < 2; ++i) {
      const word oamAddress = oamScanIndex * 4;
      const byte objectY = oam[oamAddress];

      const byte objectHeightPixels =
          bitIsSet(lcdc, PPU_LCDC_OBJ_SIZE_FLAG) ? 16 : 8;

      // Check if object overlaps the current scanline
      if (ly >= static_cast<int>(objectY) - 16 &&
          ly < static_cast<int>(objectY) - 16 + objectHeightPixels &&
          objectAttributes.size() < PPU_MAX_OBJECTS_PER_SCANLINE) {

        const byte objectAtts = oam[oamAddress + 3];

        objectAttributes.emplace_back(
            objectY, oam[oamAddress + 1], oam[oamAddress + 2],
            bitIsSet(objectAtts, PPU_OA_PRIORITY_FLAG),
            bitIsSet(objectAtts, PPU_OA_Y_FLIP_FLAG),
            bitIsSet(objectAtts, PPU_OA_X_FLIP_FLAG),
            bitIsSet(objectAtts, PPU_OA_PALETTE_FLAG));
      }

      ++oamScanIndex;
    }
  }

  // Transition after 80 dots
  if (dots == PPU_DOTS_OAM_SCAN) {
    setState(PPUState::DRAW_PIXELS);
  }
}

void PPU::hblank() {
  if (dots != PPU_DOTS_PER_SCANLINE) {
    return;
  }

  ++ly;
  checkLycInterrupt();

  if (ly < PPU_SCANLINE_VBLANK_START) {
    oamScanIndex = 0;
    objectAttributes.clear();
    setState(PPUState::OAM_SCAN);
  } else {
    // Request vblank interrupt
    byte iff = bus.read(MEMORY_IF_ADDRESS);

    setBit(iff, INTERRUPT_VBLANK_FLAG, true);

    if (bitIsSet(stat, PPU_STAT_MODE_1_INT_FLAG)) {
      setBit(iff, INTERRUPT_LCD_FLAG, true);
    }

    bus.write(MEMORY_IF_ADDRESS, iff);
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
    objectAttributes.clear();
    setState(PPUState::OAM_SCAN);
  }

  checkLycInterrupt();
  dots = 0;
}

// Sets the state of the PPU, and the lower 2 bits of the STAT register
void PPU::setState(PPUState s) {
  const byte statBits = static_cast<byte>(s) & MASK_2_LOWER_BYTE;
  stat &= ~MASK_2_LOWER_BYTE;
  stat |= statBits;
  state = s;
}

// When LYC = LY, bit 2 of the STAT register is set
// This should be run everytime LY or LYC is changed
void PPU::checkLycInterrupt() {
  const bool coincidence = ly == lyc;

  setBit(stat, PPU_STAT_LYC_LY_FLAG, coincidence);

  // Request STAT interrupt if LYC interrupt is enabled
  if (coincidence && bitIsSet(stat, PPU_STAT_LYC_INT_FLAG)) {
    byte iff = bus.read(MEMORY_IF_ADDRESS);
    setBit(iff, INTERRUPT_LCD_FLAG, true);
    bus.write(MEMORY_IF_ADDRESS, iff);
  }
}

}  // namespace gbdmge
