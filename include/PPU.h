#ifndef GBDMGE_PPU_H
#define GBDMGE_PPU_H

#include <array>
#include "Constants.h"
#include "Types.h"

namespace gbdmge {
class PPU {
 public:
  PPU() = default;
  byte read(word address);
  void write(word address, byte value);
  void reset();  // Initialize to post boot rom state

 private:
  std::array<byte, MEMORY_VRAM_END - MEMORY_VRAM_START + 1> vram;
};
}  // namespace gbdmge

#endif
