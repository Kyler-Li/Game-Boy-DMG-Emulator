#include "cartridge.h"
#include <cstring>
#include <fstream>
#include <iostream>

namespace gbdmge {

auto Cartridge::load(const std::filesystem::path& romPath) -> bool {
  auto f = std::ifstream{romPath, std::ios::binary};
  if (!f.is_open()) {
    std::cerr << "Failed to load cartridge " << romPath << ": "
              << std::strerror(errno) << "\n";
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