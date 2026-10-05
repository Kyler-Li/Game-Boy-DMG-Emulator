#ifndef GBDMGE_CARTRIDGE_H
#define GBDMGE_CARTRIDGE_H

#include <filesystem>
#include <vector>
#include "constants.h"
#include "types.h"

namespace gbdmge {
class Cartridge {
 public:
  enum class Mapper {
    None,
    MBC1,
    MBC2,
    MBC3,
    MBC4,
    MBC5,
    MBC6,
    MBC7,
    MMM01,
    M161,
    HUC1,
    HUC3
  };

  Cartridge() = default;

  auto load(const std::filesystem::path& romPath) -> bool;
  auto read(word address) -> byte;
  void write(word address, byte value);

 private:
  auto readNoMBC(word address) -> byte;
  void writeNoMBC(word address, byte value);

  auto readMBC1(word address) -> byte;
  void writeMBC1(word address, byte value);

  void incrementBankingNumber();

  std::vector<byte> rom;
  std::array<byte, MEMORY_ERAM_END - MEMORY_ERAM_START + 1> eram;
  Mapper mapper;
  byte bankingNumber;
  byte bankingMode;
  bool eRamEnabled;
};
}  // namespace gbdmge

#endif