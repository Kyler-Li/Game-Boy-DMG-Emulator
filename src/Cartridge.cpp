#include "Cartridge.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <span>
#include "Constants.h"

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
      eram.resize(CARTRIDGE_ERAM_BANK_SIZE * (1 << (2 * eRamSize - 4)));
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
      std::cerr << "Failed to read " << std::hex << address
                << " from cartridge: Unimplemented mapper\n";
      return 0xFF;
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
      return;
  }
}

auto Cartridge::readNoMBC(const word address) const -> byte {
  if (MEMORY_FIXED_BANK_START <= address && address <= MEMORY_SWITCH_BANK_END) {
    return rom[address];
  }

  if (MEMORY_ERAM_START <= address && address <= MEMORY_ERAM_END) {
    return eram[address & MASK_13_LOWER_WORD];
  }

  return 0xFF;
}

void Cartridge::writeNoMBC(const word address, const byte value) {
  if (MEMORY_ERAM_START <= address && address <= MEMORY_ERAM_END) {
    eram[address & MASK_13_LOWER_WORD] = value;
  }
}

auto Cartridge::readMBC1(const word address) const -> byte {
  if (address <= MEMORY_FIXED_BANK_END) {
    return rom[address];
  }

  if (address >= MEMORY_SWITCH_BANK_START &&
      address <= MEMORY_SWITCH_BANK_END) {
    const byte romBank = bankingMode == CARTRIDGE_ERAM_BANKING_MODE
                             ? bankingNumber & MASK_5_LOWER_BYTE
                             : bankingNumber;
    return rom[address - MEMORY_SWITCH_BANK_START +
               romBank * CARTRIDGE_ROM_BANK_SIZE];
  }

  // External RAM
  if (eRamEnabled && address >= MEMORY_ERAM_START &&
      address <= MEMORY_ERAM_END) {
    const byte ramBank = bankingMode == CARTRIDGE_ERAM_BANKING_MODE
                             ? (bankingNumber & MASK_5_6_BYTE) >> 5
                             : 0;
    return eram[address - MEMORY_ERAM_START +
                ramBank * CARTRIDGE_ERAM_BANK_SIZE];
  }

  // External RAM is only accessible if is enabled, otherwise reads return open
  // bus values (often $FF, but not guaranteed) and writes are ignored.
  return 0xFF;
}

void Cartridge::writeMBC1(const word address, const byte value) {
  if (address <= CARTRIDGE_ERAM_ENABLE_END) {
    eRamEnabled = (value & MASK_4_LOWER_BYTE) == CARTRIDGE_ERAM_ENABLE_VALUE;
  } else if (address >= CARTRIDGE_ROM_BANK_NUMBER_START &&
             address <= CARTRIDGE_ROM_BANK_NUMBER_END) {
    bankingNumber = bankingNumber & MASK_5_6_BYTE | value & MASK_5_LOWER_BYTE;
    incrementBankingNumber();
  } else if (address >= CARTRIDGE_ERAM_BANK_NUMBER_START &&
             address <= CARTRIDGE_ERAM_BANK_NUMBER_END) {
    bankingNumber = bankingNumber & ~MASK_5_6_BYTE | (value & MASK_2_LOWER_BYTE)
                                                         << 5;
    incrementBankingNumber();
  } else if (address >= CARTRIDGE_ROM_ERAM_MODE_SELECT_START &&
             address <= CARTRIDGE_ROM_ERAM_MODE_SELECT_END) {
    bankingMode = value & MASK_1_LOWER_BYTE;
  } else if (eRamEnabled && address >= MEMORY_ERAM_START &&
             address <= MEMORY_ERAM_END) {
    const byte ramBank = bankingMode == CARTRIDGE_ERAM_BANKING_MODE
                             ? (bankingNumber & MASK_5_6_BYTE) >> 5
                             : 0;
    eram[address - MEMORY_ERAM_START + ramBank * CARTRIDGE_ERAM_BANK_SIZE] =
        value;
  }
}

void Cartridge::incrementBankingNumber() {
  switch (bankingNumber) {
    case 0x0:
    case 0x20:
    case 0x40:
    case 0x60:
      ++bankingNumber;
    default:
      break;
  }
}

}  // namespace gbdmge