#include "cartridge.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <span>
#include "constants.h"

namespace gbdmge {

auto Cartridge::load(const std::filesystem::path& romPath) -> bool {
  auto f = std::ifstream{romPath, std::ios::binary};
  if (!f.is_open()) {
    std::cerr << "Failed to load cartridge: " << romPath
              << " cannot be opened, " << std::strerror(errno) << "\n";
    return false;
  }

  const auto fileSize = std::filesystem::file_size(romPath);
  rom.resize(fileSize);
  f.read(reinterpret_cast<char*>(rom.data()), fileSize);

  if (NINTENDO_LOGO_START < 0 || NINTENDO_LOGO_END >= rom.size()) {
    std::cerr << "Failed to load cartridge: ROM is invalid (too small)\n";
    return false;
  }

  const auto logo = std::span{rom}.subspan(
      NINTENDO_LOGO_START, NINTENDO_LOGO_END - NINTENDO_LOGO_START + 1);

  if (!std::ranges::equal(NINTENDO_LOGO, logo)) {
    std::cerr << "Failed to load cartridge: ROM is invalid (logo mismatch)\n";
    return false;
  }

  return true;
}

auto Cartridge::read(word address) -> byte& {
  // TODO: Implement
}

void Cartridge::write(word address, byte value) {
  // TODO: Implement
}
}  // namespace gbdmge