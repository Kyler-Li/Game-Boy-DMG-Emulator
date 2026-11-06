#ifndef GBDMGE_CARTRIDGE_H
#define GBDMGE_CARTRIDGE_H

#include <filesystem>
#include <vector>
#include "Types.h"

namespace gbdmge {
class Cartridge {
 public:
    /*
    MBC1:
        The foundation for other memory bank controllers.

        7 bit rom banking number register: indexes into rom. Set to 1 on boot. Interpreted as 5 or 7 bits, more on that below.
        2 bit eram banking number register: indexes into eram. Set to 0 on boot. Interpreted as 0 or 2 bits, more on that below.

        Example using banking number:
        To read a byte in eram the address calculation would be (into a flat sequence of eram bytes):
        (address - 0xA000) + (eram banking number * (0xBFFF - 0xA000 + 1)).
        Note that 0xA000, 0xBFFF are the starting and ending addresses for the eram banks.
        *Think of banking number telling you how many blocks of bytes can be mapped into the starting to ending address range.
        The banking number just helps you find the correct block of bytes.

        eram is disabled on boot.

        Two banking modes:
        Banking mode = 0
        ROM is limited to 7 bits of rom banking number, banks 0x0 - 0x7F - for READING.
        ERAM is limited to 0 bits of eram banking number, banks 0x0 ONLY - for READING and WRITING.

        Banking mode = 1
        ROM is limited to 5 bits of rom banking number, banks 0x0 - 0x1F - for READING.
        ERAM is limited to 2 bits of eram banking number, banks 0x0 - 0x3 - for READING and WRITING.

        Writing a value to 0x4000 - 0x5FFF
        If eram was enabled by writing a value that contained 0x0A to 0x0000 - 0x1FFF
        previously, then the lower 2 bits of the eram banking number is set to the lower
        2 bits of the value. If eram is disabled, the lower 2 bits of the value are written
        to bits 5 and 6 of the rom banking number.

        Writing a value to 0x2000 - 0x3FFF
        Sets the lower 5 bits of the rom banking number to the lower 5 bits of the
        value.

        Writing to 0x6000 - 0x7FFF
        Sets the banking mode to the 0th bit of the value.

        When the rom banking number is set it in some fashion must be checked to see if it is
        0x0, 0x20, 0x40, or 0x60 and subsequently by 1. This is because the rom banking number
        could be 0 multiple times and needs to be in the range 01-1Fh. To see why this needs
        to happen suppose the rom banking number is 0x60 or 01100000 in binary and our banking
        mode is 1. Since the banking mode is 1, that means the lower 5 bits of the rom banking
        number would be selected for operations when reading rom. In the case of 0x60, the
        lower 5 bits are 00000 which would be invalid for rom reads.
    MBC3:
        TODO
    */
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
  auto readNoMBC(word address) const -> byte;
  void writeNoMBC(word address, byte value);

  auto readMBC1(word address) const -> byte;
  void writeMBC1(word address, byte value);

  auto readMBC3(word address) const -> byte;
  void writeMBC3(word address, byte value);
  void updateRtcLatchMBC3();

  void incrementBankingNumber();

  std::vector<byte> rom;
  std::vector<byte> eRam;
  Mapper mapper;
  byte eRamBankingNumber;
  byte romBankingNumber;
  byte bankingMode;
  byte rtcSeconds;
  byte rtcMinutes;
  byte rtcHours;
  byte rtcDaysLow;
  byte rtcDaysHigh;
  byte rtcLatch;
  bool eRamEnabled;
};
}  // namespace gbdmge

#endif