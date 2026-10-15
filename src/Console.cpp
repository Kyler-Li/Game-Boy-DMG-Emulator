#include "Console.h"
#include <iostream>

namespace gbdmge {
Console::Console() : bus{*this}, cpu{bus} {}

auto Console::load(const std::filesystem::path& romPath) -> bool {
  cpu.reset();
  return cartridge.load(romPath);
}

void Console::step() {
  cpu.cycle();
}

}  // namespace gbdmge