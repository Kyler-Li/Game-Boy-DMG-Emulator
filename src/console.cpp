#include "console.h"

namespace gbdmge {
Console::Console() : bus{*this}, cpu{bus} {}

auto Console::load(const std::filesystem::path& romPath) -> bool {
  return cartridge.load(romPath);
}

void Console::play() {
  // TODO: Implement
}

}  // namespace gbdmge