#ifndef GBDMGE_CONSOLE_H
#define GBDMGE_CONSOLE_H

#include "Audio.h"
#include "Bus.h"
#include "CPU.h"
#include "Cartridge.h"
#include "Joypad.h"
#include "PPU.h"
#include "Timer.h"

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
  Timer timer;
  word iff;  // Interrupt flag
  word ie;   // Interrupt enable
  std::array<byte, MEMORY_WRAM_END - MEMORY_WRAM_START + 1> wram;
  std::array<byte, MEMORY_HRAM_END - MEMORY_HRAM_START + 1> hram;
  friend class Bus;
};
}  // namespace gbdmge

#endif