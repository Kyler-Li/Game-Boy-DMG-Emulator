#include "Console.h"

namespace gbdmge {
Console::Console() : bus{*this}, cpu{bus} {}

auto Console::load(const std::filesystem::path& romPath) -> bool {
  return cartridge.load(romPath);
}

void Console::step() {
  const auto opcode = cpu.fetchByte();
  const auto cycles = cpu.execute(opcode);
}

}  // namespace gbdmge