#ifndef GBDMGE_CONSOLE_H
#define GBDMGE_CONSOLE_H

#include "Audio.h"
#include "Bus.h"
#include "Cartridge.h"
#include "CPU.h"
#include "Joypad.h"
#include "PPU.h"

namespace gbdmge {
class Console {
 public:
  Console();

  auto load(const std::filesystem::path& romPath) -> bool;
  void step();

 private:
  Bus bus;
  CPU cpu;
  Cartridge cartridge;
  Joypad joypad;
  Audio audio;
  PPU ppu;
  std::array<byte, MEMORY_SWITCH_WRAM_END - MEMORY_WRAM_START + 1> wram;
  friend class Bus;
};
}  // namespace gbdmge

#endif