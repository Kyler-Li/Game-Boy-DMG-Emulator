#ifndef GBDMGE_PPU_H
#define GBDMGE_PPU_H

#include <array>
#include <vector>

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
  DRAW_PIXELS = 3,
  H_BLANK = 0,
  V_BLANK = 1,
};

class PPU {
 public:
  PPU();
  byte read(word address);
  void write(word address, byte value);
  void reset();  // Initialize to post boot rom state
  void tick(qword cycles);
  void oamScan();
  void hblank();
  void vblank();
  void setState(PPUState s);

 private:
  PPUState state;
  // 1 dot = 1 T-cycle
  qword dots;
  byte oamScanIndex;

  byte lcdc;
  byte stat;
  byte ly;
  byte lyc;
  byte scx;
  byte scy;
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
