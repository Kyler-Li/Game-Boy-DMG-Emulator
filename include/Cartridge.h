#ifndef GBDMGE_CARTRIDGE_H
#define GBDMGE_CARTRIDGE_H

#include <filesystem>
#include <vector>
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

  Cartridge();

  auto load(const std::filesystem::path& romPath) -> bool;
  auto read(word address) const -> byte;
  void write(word address, byte value);

 private:
  bool hasNintendoLogo(std::size_t offset);

  auto readNoMBC(word address) const -> byte;
  void writeNoMBC(word address, byte value);

  auto readMBC1(word address) const -> byte;
  void writeMBC1(word address, byte value);
  bool isMulticart();

  auto readMBC3(word address) const -> byte;
  void writeMBC3(word address, byte value);
  void updateRtcLatchMBC3();

  auto readMBC5(word address) const -> byte;
  void writeMBC5(word address, byte value);

  std::vector<byte> rom;
  std::vector<byte> eRam;

  Mapper mapper;

  byte eRamBankingNumber;
  word romBankingNumber;
  byte bankingMode;

  byte rtcSeconds;
  byte rtcMinutes;
  byte rtcHours;
  byte rtcDaysLow;
  byte rtcDaysHigh;
  byte rtcLatch;

  bool eRamEnabled;
  bool multicartEnabled;
};
}  // namespace gbdmge

#endif