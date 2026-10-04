#include "cartridge.h"
#include <fstream>

namespace gbdmge {

auto Cartridge::load(const std::filesystem::path& romPath) -> bool {
  auto f = std::fstream{romPath, std::ios::binary};
  if (!f.is_open()) {
    return false;
  }

  f.read(reinterpret_cast<char*>(rom.data()),
         std::filesystem::file_size(romPath));
  return true;
}

auto Cartridge::read(word address) -> byte& {
  // TODO: Implement
}

void Cartridge::write(word address, byte value) {
  // TODO: Implement
}
}  // namespace gbdmge