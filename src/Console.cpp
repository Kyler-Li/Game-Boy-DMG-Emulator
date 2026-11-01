#include "Console.h"

namespace gbdmge {
Console::Console()
    : bus{*this},
      cpu{bus},
      joypad{},
      audio{},
      ppu{},
      iff{0},
      ie{0},
      wram{},
      hram{} {}

auto Console::load(const std::filesystem::path& romPath) -> bool {
  reset();
  return cartridge.load(romPath);
}

void Console::tick() {
  const qword cycles = cpu.tick();
  ppu.tick(cycles);
}

void Console::reset() {
  cpu.reset();
  ppu.reset();
  audio.reset();
  joypad.reset();
  timer.reset();
}

}  // namespace gbdmge
