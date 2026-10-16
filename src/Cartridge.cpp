#include "Cartridge.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <span>
#include <chrono>
#include "Constants.h"

namespace gbdmge {

Cartridge::Cartridge()
    : mapper{Mapper::None},
      eRamBankingNumber{0},
      romBankingNumber{1},
      bankingMode{CARTRIDGE_ROM_BANKING_MODE},
      rtcSeconds{0},
      rtcMinutes{0},
      rtcHours{0},
      rtcDaysLow{0},
      rtcDaysHigh{0},
      rtcLatch{0},
      eRamEnabled{false} {}

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

  // Pan Docs list that a rom size value >= 52 exists and doesn't fit this formula but,
  // no cartridges or ROM files using the rom sizes corresponding to the rom values exist.
  // Ensure that the ROM size (which was already loaded) matches the one specified in the file
  if (const dword estimatedRomSize =
          2 * CARTRIDGE_ROM_BANK_SIZE * (1 << rom[CARTRIDGE_ROM_SIZE_VALUE]);
      fileSize < estimatedRomSize) {
    std::cerr << "Failed to load cartridge: ROM has invalid size\n";
    return false;
  }

  if (NINTENDO_LOGO_END >= rom.size()) {
    std::cerr << "Failed to load cartridge: ROM is invalid (too small)\n";
    return false;
  }

  const auto logo = std::span{rom}.subspan(
      NINTENDO_LOGO_START, NINTENDO_LOGO_END - NINTENDO_LOGO_START + 1);

  if (!std::ranges::equal(NINTENDO_LOGO, logo)) {
    std::cerr << "Failed to load cartridge: ROM is invalid (logo mismatch)\n";
    return false;
  }

  switch (rom[CARTRIDGE_TYPE_START]) {
    case 0x00:
      mapper = Mapper::None;
      break;
    case 0x01:
    case 0x02:
    case 0x03:
      mapper = Mapper::MBC1;
      break;
    case 0x05:
    case 0x06:
      mapper = Mapper::MBC2;
      break;
    case 0x0F:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
      mapper = Mapper::MBC3;
      break;
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
      mapper = Mapper::MBC5;
      break;
    case 0x20:
      mapper = Mapper::MBC6;
      break;
    case 0x22:
      mapper = Mapper::MBC7;
      break;
    case 0xFE:
      mapper = Mapper::HUC3;
      break;
    case 0xFF:
      mapper = Mapper::HUC1;
      break;
    default:
      std::cerr << "Failed to load cartridge: Unrecognized cartridge type\n";
      return false;
  }

  // Set eram size
  switch (const byte eRamSize = rom[CARTRIDGE_ERAM_SIZE_START]) {
    case 0x00:
      break;
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:  // Case 0x05 doesn't fit this pattern but, extra space doesn't hurt too much
      eRam.resize(CARTRIDGE_ERAM_BANK_SIZE * (1 << (2 * eRamSize - 4)));
      break;
    default:
      std::cerr << "Failed to load cartridge: Unrecognized ERAM size\n";
      return false;
  }

  return true;
}

auto Cartridge::read(const word address) const -> byte {
  switch (mapper) {
    case Mapper::None:
      return readNoMBC(address);
    case Mapper::MBC1:
      return readMBC1(address);
    case Mapper::MBC3:
      return readMBC3(address);
    default:
      assert(false && "Cartridge read failed, unrecognized mapper");
  }
}

void Cartridge::write(const word address, const byte value) {
  switch (mapper) {
    case Mapper::None:
      writeNoMBC(address, value);
      return;
    case Mapper::MBC1:
      writeMBC1(address, value);
      return;
    case Mapper::MBC3:
      writeMBC3(address, value);
      return;
    default:
      assert(false && "Cartridge write failed, unrecognized mapper");
  }
}

auto Cartridge::readNoMBC(const word address) const -> byte {
  if (MEMORY_FIXED_BANK_START <= address && address <= MEMORY_SWITCH_BANK_END) {
    return rom[address];
  }

  if (MEMORY_ERAM_START <= address && address <= MEMORY_ERAM_END) {
    return eRam[address & MASK_13_LOWER_WORD];
  }

  return 0xFF;
}

void Cartridge::writeNoMBC(const word address, const byte value) {
  if (MEMORY_ERAM_START <= address && address <= MEMORY_ERAM_END) {
    eRam[address & MASK_13_LOWER_WORD] = value;
  }
}

auto Cartridge::readMBC1(const word address) const -> byte {
  // 0x0000 - 0x3FFF
  if (address <= MEMORY_FIXED_BANK_END) {
    return rom[address];
  }

  // 0x4000 -  0x7FFF
  if (address >= MEMORY_SWITCH_BANK_START &&
      address <= MEMORY_SWITCH_BANK_END) {
    const byte romBank =
        bankingMode == CARTRIDGE_ERAM_BANKING_MODE
            ? romBankingNumber & MASK_5_LOWER_BYTE   // Address 512KB of ROM
            : romBankingNumber & MASK_7_LOWER_BYTE;  // Address 2MB of ROM
    // 0x4000, 0x4000
    return rom[address - MEMORY_SWITCH_BANK_START +
               romBank * CARTRIDGE_ROM_BANK_SIZE];
  }

  // 0xA000 - 0xBFFF
  if (eRamEnabled && address >= MEMORY_ERAM_START &&
      address <= MEMORY_ERAM_END) {
    const byte ramBank = bankingMode == CARTRIDGE_ERAM_BANKING_MODE
                             ? eRamBankingNumber & MASK_2_LOWER_BYTE
                             : 0;
    // 0xA000, 0x2000
    return eRam[address - MEMORY_ERAM_START +
                ramBank * CARTRIDGE_ERAM_BANK_SIZE];
  }

  // External RAM is only accessible if is enabled, otherwise reads return open
  // bus values (often $FF, but not guaranteed) and writes are ignored.
  return 0xFF;
}

void Cartridge::writeMBC1(const word address, const byte value) {
  // 0x1FFF
  if (address <= CARTRIDGE_ERAM_ENABLE_END) {
    eRamEnabled = (value & MASK_4_LOWER_BYTE) == CARTRIDGE_ERAM_ENABLE_VALUE;

    // 0x2000 - 0x3FFF
  } else if (address >= CARTRIDGE_ROM_BANK_NUMBER_START &&
             address <= CARTRIDGE_ROM_BANK_NUMBER_END) {
    romBankingNumber =
        romBankingNumber & MASK_5_6_BYTE | value & MASK_5_LOWER_BYTE;
    incrementBankingNumber();

    // 0x4000 - 0x5FFF
  } else if (address >= CARTRIDGE_ERAM_BANK_NUMBER_START &&
             address <= CARTRIDGE_ERAM_BANK_NUMBER_END) {
    if (eRamEnabled) {
      eRamBankingNumber = value & MASK_2_LOWER_BYTE;
    } else {
      romBankingNumber =
          romBankingNumber & ~MASK_5_6_BYTE | (value & MASK_2_LOWER_BYTE) << 5;
      incrementBankingNumber();
    }

    // 0x6000 - 0x7FFF
  } else if (address >= CARTRIDGE_ROM_ERAM_MODE_SELECT_START &&
             address <= CARTRIDGE_ROM_ERAM_MODE_SELECT_END) {
    bankingMode = value & MASK_1_LOWER_BYTE;

    // 0xA000 - 0xBFFF
  } else if (eRamEnabled && address >= MEMORY_ERAM_START &&
             address <= MEMORY_ERAM_END) {
    const byte ramBank = bankingMode == CARTRIDGE_ERAM_BANKING_MODE
                             ? eRamBankingNumber & MASK_2_LOWER_BYTE
                             : 0;
    // 0xA000, 0x2000
    eRam[address - MEMORY_ERAM_START + ramBank * CARTRIDGE_ERAM_BANK_SIZE] =
        value;
  }
}

auto Cartridge::readMBC3(const word address) const -> byte {
  // 0x0000 - 0x3FFF
  if (address <= MEMORY_FIXED_BANK_END) {
    return rom[address];
  }

  // 0x4000 -  0x7FFF
  if (address >= MEMORY_SWITCH_BANK_START &&
    address <= MEMORY_SWITCH_BANK_END) {
    // 0x4000, 0x4000
    return rom[address - MEMORY_SWITCH_BANK_START +
               romBankingNumber * CARTRIDGE_ROM_BANK_SIZE];
  }

  // 0xA000 - 0xBFFF
  if (eRamEnabled && address >= MEMORY_ERAM_START &&
      address <= MEMORY_ERAM_END) {
      switch (eRamBankingNumber) {
        case 0x0:
        case 0x1:
        case 0x2:
        case 0x3:
          return eRam[address - MEMORY_ERAM_START +
                eRamBankingNumber * CARTRIDGE_ERAM_BANK_SIZE];
        case 0x8: return rtcSeconds;
        case 0x9: return rtcMinutes;
        case 0xA: return rtcHours;
        case 0xB: return rtcDaysLow;
        case 0xC: return rtcDaysHigh;
        default:
          return 0xFF;
      }
  }

  return 0xFF;
}

void Cartridge::writeMBC3(const word address, const byte value) {
  // 0x0000 - 0x1FFF
  // If eram is enabled, so is RTC
  if (address <= CARTRIDGE_ERAM_ENABLE_END) {
    eRamEnabled = (value & MASK_4_LOWER_BYTE) == CARTRIDGE_ERAM_ENABLE_VALUE;

    // 0x2000 - 0x3FFF
  }else if (address >= CARTRIDGE_ROM_BANK_NUMBER_START &&
             address <= CARTRIDGE_ROM_BANK_NUMBER_END) {
    romBankingNumber = value & MASK_7_LOWER_BYTE;
    if (romBankingNumber == 0) {
      romBankingNumber = 1;
    }

    // 0x4000 - 0x5FFF
  }else if (address >= CARTRIDGE_ERAM_BANK_NUMBER_START &&
             address <= CARTRIDGE_ERAM_BANK_NUMBER_END) {
    eRamBankingNumber = value & MASK_4_LOWER_BYTE;

  // 0x6000 - 0x7FFF
  }else if (address >= CARTRIDGE_ROM_ERAM_MODE_SELECT_START &&
                        address <= CARTRIDGE_ROM_ERAM_MODE_SELECT_END) {
    if (rtcLatch == 0 && value == 1) {
      updateRtcLatchMBC3();
    }
    rtcLatch = value;
  // 0xA000 - 0xBFFF
  }else if (eRamEnabled && address >= MEMORY_ERAM_START &&
                 address <= MEMORY_ERAM_END) {
      switch (eRamBankingNumber) {
        case 0x0:
        case 0x1:
        case 0x2:
        case 0x3:
          eRam[address - MEMORY_ERAM_START +
               eRamBankingNumber * CARTRIDGE_ERAM_BANK_SIZE] = value & MASK_4_LOWER_BYTE;
          break;
        case 0x8: rtcSeconds = value & MASK_6_LOWER_BYTE; break;
        case 0x9: rtcMinutes = value & MASK_6_LOWER_BYTE; break;
        case 0xA: rtcHours = value & MASK_5_LOWER_BYTE; break;
        case 0xB: rtcDaysLow = value; break;
        case 0xC: rtcDaysHigh = value; break;
        default:
          assert(false && "Unknown eram banking number found when writing with MBC3.");
      }
   }
}

void Cartridge::updateRtcLatchMBC3(){
  // Uses time since epoch, update later with an accurate counter
  const auto now = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch()
  ).count();

  rtcSeconds = static_cast<byte>(now % 60);
  rtcMinutes = static_cast<byte>(now / 60 % 60);
  rtcHours = static_cast<byte>(now / 3600 % 24);

  const word days = static_cast<word>(now / 86400);
  rtcDaysLow = static_cast<byte>(days & MASK_8_LOWER_WORD);

  // Upper bits and flags in rtcDaysHigh.
  rtcDaysHigh = rtcDaysHigh & MASK_8_UPPER_BYTE  | static_cast<byte>(days >> 8 & MASK_1_LOWER_WORD);

  if (days > 511){
    rtcDaysHigh |= MASK_1_UPPER_BYTE;
  }
}

void Cartridge::incrementBankingNumber() {
  switch (romBankingNumber) {
    case 0x0:
    case 0x20:
    case 0x40:
    case 0x60:
      ++romBankingNumber;
    default:
      break;
  }
}

}  // namespace gbdmge