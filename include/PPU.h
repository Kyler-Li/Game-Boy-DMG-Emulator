#ifndef GBDMGE_PPU_H
#define GBDMGE_PPU_H

#include <array>
#include <vector>

#include "Bus.h"
#include "Constants.h"
#include "Types.h"

namespace gbdmge {

// Struct to hold object attributes from OAM and other information
struct ObjectAttribute {
  byte y;
  byte x;
  byte tileIndex;

  // These booleans represent the bits actually used from byte 3 of an object's attributes
  bool priority;
  bool yFlip;
  bool xFlip;
  bool palette;
};

enum class PPUState {
  OAM_SCAN = 2,
  PIXEL_TRANSFER = 3,
  H_BLANK = 0,
  V_BLANK = 1,
};

enum class PPUTileMapSource {
  WINDOW,
  BACKGROUND,
  OBJECT
};

enum class PPUPixelTransferState {
  GET_TILE_INDEX,
  GET_TILE_DATA_LOW,
  GET_TILE_DATA_HIGH,
  PUSH
};

class PPU {
 public:
  explicit PPU(Bus& bus);
  byte read(word address);
  void write(word address, byte value);
  void reset();  // Initialize to post boot rom state
  void tick(qword cycles);
  void oamScan();
  void pixelTransfer();
  void getTileIndex();
  void getTileDataLow();
  void getTileDataHigh();
  void hblank();
  void vblank();
  void setState(PPUState s);
  void enterOamScan();
  void checkLycInterrupt();
  void setStatLCD(byte statPos) const;

 private:
  Bus& bus;
  PPUState state;
  PPUTileMapSource tileMapSource;
  PPUPixelTransferState pixelTransferState;
  // 1 dot = 1 T-cycle
  qword dots;
  byte oamScanIndex;
  byte tileMapColumn;
  byte windowRowPixel;
  byte currentTileIndex;
  word currentTileAddress;
  byte currentTileLow;

  byte lcdc;
  byte stat;
  byte ly;
  byte lyc;
  byte scx; // in pixels
  byte scy; // in pixels
  byte wx;
  byte wy;
  byte bgp;
  byte obp0;
  byte obp1;

  std::array<byte, MEMORY_VRAM_END - MEMORY_VRAM_START + 1> vram;
  std::array<byte, MEMORY_OAM_END - MEMORY_OAM_START + 1> oam;

  std::vector<ObjectAttribute> objectAttributes;
};
}  // namespace gbdmge

#endif
