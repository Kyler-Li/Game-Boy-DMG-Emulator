#ifndef GBDMGE_CARTRIDGE_H
#define GBDMGE_CARTRIDGE_H

#include <filesystem>
#include <vector>
#include "Constants.h"
#include "Types.h"

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
  [[nodiscard]] auto read(word address) const -> byte;
  void write(word address, byte value);

 private:
  [[nodiscard]] auto readNoMBC(word address) const -> byte;
  void writeNoMBC(word address, byte value);

  [[nodiscard]] auto readMBC1(word address) const -> byte;
  void writeMBC1(word address, byte value);

  void incrementBankingNumber();

  std::vector<byte> rom;
  std::vector<byte> eram;
  Mapper mapper;
  byte bankingNumber;
  byte bankingMode{CARTRIDGE_ROM_BANKING_MODE};
  bool eRamEnabled;
};
}  // namespace gbdmge

#endif