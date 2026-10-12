#include "Console.h"
#include <iostream>

namespace gbdmge {
Console::Console() : bus{*this}, cpu{bus} {}

auto Console::load(const std::filesystem::path& romPath) -> bool {
  return cartridge.load(romPath);
}

void Console::step() {
  const auto opcode = cpu.fetchByte();
  const auto cycles = cpu.execute(opcode);
  std::cout << "ran " << "0x" << std::uppercase << std::hex
            << static_cast<unsigned int>(opcode) << ", " << std::dec << cycles
            << " cycles\n";
}

}  // namespace gbdmge