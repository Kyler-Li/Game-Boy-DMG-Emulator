#ifndef GBDMGE_CARTRIDGE_H
#define GBDMGE_CARTRIDGE_H

#include <filesystem>
#include <vector>
#include "types.h"

namespace gbdmge {
class Cartridge {
 public:
  Cartridge() = default;

  auto load(const std::filesystem::path& romPath) -> bool;
  auto read(word address) -> byte&;
  void write(word address, byte value);

 private:
  std::vector<byte> rom;
};
}  // namespace gbdmge

#endif