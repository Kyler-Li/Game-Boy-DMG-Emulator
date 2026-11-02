#include "PPU.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>

#include "bitops.h"

namespace gbdmge {
PPU::PPU(Bus& bus)
    : bus{bus},
      state{PPUState::OAM_SCAN},
      tileMapSource{PPUTileMapSource::BACKGROUND},
      pixelTransferState{PPUPixelTransferState::GET_TILE_INDEX},
      dots{0},
      oamScanIndex{0},
      tileMapColumn{0},
      windowRowPixel{0},
      currentTileIndex{0},
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
  // Read from vram, registers, etc
  return 0xFF;
}

void PPU::write(word address, byte value) {
  // TODO: Implement
  // Write to vram, registers, etc
}

void PPU::reset() {
  // TODO: Keep working on this, may need to change / extend
  // Reset PPU, and initialize registers to post boot values

  dots = 0;

  lcdc = 0x91;
  stat = 0x85;
  scy = 0x00;
  scx = 0x00;
  ly = 0x00;
  lyc = 0x00;
  wx = 0x00;
  wy = 0x00;
  obp0 = 0xFF;
  obp1 = 0xFF;
  bgp = 0xFC;

  oamScanIndex = 0;
  objectAttributes.clear();

  tileMapColumn = 0;
  tileMapSource = PPUTileMapSource::BACKGROUND;
  pixelTransferState = PPUPixelTransferState::GET_TILE_INDEX;
  windowRowPixel = 0;
  currentTileIndex = 0;

  setState(PPUState::OAM_SCAN);
}

void PPU::tick(const qword cycles) {
  for (qword i = 0; i < cycles; ++i) {
    switch (state) {
      case PPUState::OAM_SCAN:
        oamScan();
        break;

      case PPUState::PIXEL_TRANSFER:
        pixelTransfer();
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

    ++dots;
  }
}

void PPU::oamScan() {
  // Process two OAM entries every 4 dots
  if (dots > 0 && dots % 4 == 0) {
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
    // Put lowest x as first element, and preserve ordering between ties
    std::ranges::stable_sort(
        objectAttributes,
        [](const ObjectAttribute& a, const ObjectAttribute& b) {
            return a.x < b.x;
        }
    );

    tileMapColumn = 0;
    tileMapSource = PPUTileMapSource::BACKGROUND;
    pixelTransferState = PPUPixelTransferState::GET_TILE_INDEX;
    setState(PPUState::PIXEL_TRANSFER);
  }
}

void PPU::pixelTransfer() {
  switch (pixelTransferState) {
    case PPUPixelTransferState::GET_TILE_INDEX:
      getTileIndex();
      break;
    case PPUPixelTransferState::GET_TILE_DATA_LOW:
      getTileDataLow();
      break;
    case PPUPixelTransferState::GET_TILE_DATA_HIGH:
      // TODO implement
      break;
    case PPUPixelTransferState::PUSH:
      // TODO implement
      break;
    default:
      assert(false && "Impossible pixel transfer state reached.");
  }
}

void PPU::getTileIndex() {

  if (tileMapSource == PPUTileMapSource::OBJECT) {
    const ObjectAttribute& objectAttribute = objectAttributes.front();
    currentTileIndex = objectAttribute.tileIndex;

    if (bitIsSet(lcdc, PPU_LCDC_OBJ_SIZE_FLAG)) {
      currentTileIndex &= MASK_8_UPPER_BYTE;
    }

    pixelTransferState = PPUPixelTransferState::GET_TILE_DATA_LOW;
    return;
  }

  word tileMapAddress;
  byte tileX;
  byte tileY;

  if (tileMapSource == PPUTileMapSource::WINDOW) {
    tileMapAddress = bitIsSet(lcdc, PPU_LCDC_WINDOW_TILE_MAP_FLAG)
        ? PPU_TILE_MAP_1_START : PPU_TILE_MAP_0_START;

    tileX = tileMapColumn;
    tileY = windowRowPixel;

  } else if (tileMapSource == PPUTileMapSource::BACKGROUND) {
    tileMapAddress = bitIsSet(lcdc, PPU_LCDC_BG_TILE_MAP_FLAG)
        ? PPU_TILE_MAP_1_START : PPU_TILE_MAP_0_START;

    tileX = ((scx / 8) + tileMapColumn) & MASK_5_LOWER_BYTE;
    tileY = (ly + scy) & MASK_8_LOWER_BYTE;

  } else {
    assert(false && "Impossible PPU tile map source!");
  }

  tileMapAddress += tileX + ((tileY / 8) * 32);
  currentTileIndex = vram[tileMapAddress - MEMORY_VRAM_START];
  pixelTransferState = PPUPixelTransferState::GET_TILE_DATA_LOW;
}

void PPU::getTileDataLow() {
  // TODO implement
}

void PPU::hblank() {
  if (dots != PPU_DOTS_PER_SCANLINE) {
    return;
  }

  ++ly;

  if (ly < PPU_SCANLINE_VBLANK_START) {
    enterOamScan();
  } else {
    // Request vblank interrupt
    byte iff = bus.read(MEMORY_IF_ADDRESS);
    setBit(iff, INTERRUPT_VBLANK_FLAG, true);
    bus.write(MEMORY_IF_ADDRESS, iff);

    setStatLCD(PPU_STAT_MODE_1_INT_FLAG);

    setState(PPUState::V_BLANK);
  }

  checkLycInterrupt();
  dots = 0;
}

void PPU::vblank() {
  if (dots != PPU_DOTS_PER_SCANLINE) {
    return;
  }

  if (ly < PPU_SCANLINE_VBLANK_END) {
    ++ly;
  } else {
    ly = 0;
    windowRowPixel = 0;
    enterOamScan();
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

void PPU::enterOamScan() {
  oamScanIndex = 0;
  objectAttributes.clear();

  setState(PPUState::OAM_SCAN);
  setStatLCD(PPU_STAT_MODE_2_INT_FLAG);
}

// When LYC = LY, bit 2 of the STAT register is set
// This should be run everytime LY or LYC is changed
void PPU::checkLycInterrupt() {
  const bool coincidence = ly == lyc;

  setBit(stat, PPU_STAT_LYC_LY_FLAG, coincidence);

  // Request STAT interrupt if LYC interrupt is enabled
  if (coincidence) {
    setStatLCD(PPU_STAT_LYC_INT_FLAG);
  }
}

void PPU::setStatLCD(const byte statPos) const {
  if (bitIsSet(stat, statPos)) {
    byte iff = bus.read(MEMORY_IF_ADDRESS);
    setBit(iff, INTERRUPT_LCD_FLAG, true);
    bus.write(MEMORY_IF_ADDRESS, iff);
  }
}

}  // namespace gbdmge
