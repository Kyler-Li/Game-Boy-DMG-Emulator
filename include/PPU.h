#ifndef GBDMGE_PPU_H
#define GBDMGE_PPU_H

#include <array>
#include "Constants.h"
#include "Types.h"

namespace gbdmge {

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
  void hblank();
  void vblank();
  void setState(PPUState s);

 private:
  PPUState state;
  // 1 dot = 1 T-cycle
  qword dots;
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
};
}  // namespace gbdmge

#endif
