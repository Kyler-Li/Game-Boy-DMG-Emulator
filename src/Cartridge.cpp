#include "Cartridge.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <span>
#include "Constants.h"

namespace gbdmge {

Cartridge::Cartridge()
    : mapper{Mapper::None},
      eRamBankingNumber{0},
      romBankingNumber{1},
      bankingMode{CARTRIDGE_ROM_BANKING_MODE},
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

  // Set rom and eram size
  switch (const byte romSize = rom[CARTRIDGE_ROM_SIZE_START]) {
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
    case 0x08:
      rom.resize(CARTRIDGE_ROM_BANK_SIZE * (1 << (romSize + 1)));
      break;
    default:
      throw std::runtime_error("Cannot determine ROM size!");
  }

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
      throw std::runtime_error("Cannot determine ERAM size!");
  }

  return true;
}

auto Cartridge::read(const word address) const -> byte {
  switch (mapper) {
    case Mapper::None:
      return readNoMBC(address);
    case Mapper::MBC1:
      return readMBC1(address);
    default:
      throw std::runtime_error(
          "Failed to read from cartridge, unimplemented mapper!");
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
    default:
      throw std::runtime_error(
          "Failed to read from cartridge, unimplemented mapper!");
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
  // 0x3FFF
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