#include "Console.h"

namespace gbdmge {
Console::Console()
    : bus{*this},
      cpu{bus},
      joypad{},
      apu{},
      ppu{},
      iff{0},
      ie{0},
      wram{},
      hram{} {}

auto Console::load(const std::filesystem::path& romPath) -> bool {
  reset();
  return cartridge.load(romPath);
}

void Console::step() {
  cpu.cycle();
}

void Console::reset() {
  cpu.reset();
  ppu.reset();
  apu.reset();
  joypad.reset();
  timer.reset();
}

}  // namespace gbdmge
