#include <algorithm>
#include <cassert>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <span>
#include "Cartridge.h"
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
      eRamEnabled{false},
      multicartEnabled{false} {}

auto Cartridge::load(const std::filesystem::path& romPath) -> bool {
    auto f = std::ifstream{romPath, std::ios::binary};

    if (!f.is_open()) {
        std::cerr << "Failed to load cartridge: " << romPath
                  << " cannot be opened, " << std::strerror(errno) << "\n";
        return false;
    }

    const auto fileSize = std::filesystem::file_size(romPath);
    rom.resize(fileSize);
    f.read(reinterpret_cast<char*>(rom.data()), static_cast<std::streamsize>(fileSize));

    if (!f) {
        std::cerr << "Failed to load cartridge: ROM could not be read\n";
        return false;
    }

    // Validate that the ROM contains the required header fields.
    if (rom.size() <= CARTRIDGE_ROM_SIZE_VALUE || rom.size() <= CARTRIDGE_ERAM_SIZE_CODE) {
        std::cerr << "Failed to load cartridge: ROM is invalid (too small)\n";
        return false;
    }

    // Determine expected ROM size.
    std::size_t estimatedRomSize = 0;
    if (const byte romSizeCode = rom[CARTRIDGE_ROM_SIZE_VALUE];
      romSizeCode <= CARTRIDGE_ROM_SIZE_VALUE_8MB) {
        estimatedRomSize = 2 * CARTRIDGE_ROM_BANK_SIZE * (1 << romSizeCode);
    } else {
        switch (romSizeCode) {
            case 0x52:
                estimatedRomSize = 72 * CARTRIDGE_ROM_BANK_SIZE;
                break;
            case 0x53:
                estimatedRomSize = 80 * CARTRIDGE_ROM_BANK_SIZE;
                break;
            case 0x54:
                estimatedRomSize = 96 * CARTRIDGE_ROM_BANK_SIZE;
                break;
            default:
                std::cerr << "Failed to load cartridge: Unrecognized ROM size\n";
                return false;
        }
    }

    if (fileSize < estimatedRomSize) {
        std::cerr << "Failed to load cartridge: ROM has invalid size\n";
        return false;
    }

    if (NINTENDO_LOGO_END >= rom.size()) {
        std::cerr << "Failed to load cartridge: ROM is invalid (too small)\n";
        return false;
    }

    if (!hasNintendoLogo(NINTENDO_LOGO_START)) {
        std::cerr << "Failed to load cartridge: ROM is invalid (logo mismatch)\n";
        return false;
    }

    switch (rom[CARTRIDGE_TYPE]) {
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

    switch (rom[CARTRIDGE_ERAM_SIZE_CODE]) {
        case 0x00:
            break;
        case 0x01:
            eRam.resize(2 * CARTRIDGE_ERAM_BASE_SIZE_BYTES);
            break;
        case 0x02:
            eRam.resize(8 * CARTRIDGE_ERAM_BASE_SIZE_BYTES);
            break;
        case 0x03:
            eRam.resize(32 * CARTRIDGE_ERAM_BASE_SIZE_BYTES);
            break;
        case 0x04:
        case 0x05:
            if (mapper == Mapper::MBC1) {
                std::cerr << "Failed to load cartridge: Unsupported MBC1 ERAM size\n";
                return false;
            }

            if (rom[CARTRIDGE_ERAM_SIZE_CODE] == CARTRIDGE_ERAM_SIZE_CODE_128KB) {
                eRam.resize(128 * CARTRIDGE_ERAM_BASE_SIZE_BYTES);
            } else {
                eRam.resize(64 * CARTRIDGE_ERAM_BASE_SIZE_BYTES);
            }
            break;
        default:
            std::cerr << "Failed to load cartridge: Unrecognized ERAM size\n";
            return false;
    }

    // Reset mapper registers.
    romBankingNumber = 1;
    eRamBankingNumber = 0;
    bankingMode = CARTRIDGE_ROM_BANKING_MODE;
    eRamEnabled = false;
    multicartEnabled = isMulticart();

    rtcSeconds = 0;
    rtcMinutes = 0;
    rtcHours = 0;
    rtcDaysLow = 0;
    rtcDaysHigh = 0;
    rtcLatch = 0;
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
    case Mapper::MBC5:
      return readMBC5(address);
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
    case Mapper::MBC5:
      return writeMBC5(address, value);
    default:
      assert(false && "Cartridge write failed, unrecognized mapper");
  }
}

bool Cartridge::hasNintendoLogo(const std::size_t offset) {
  if (offset + NINTENDO_LOGO.size() > rom.size()) {
    return false;
  }

  return std::ranges::equal(NINTENDO_LOGO,
    std::span{rom}.subspan(offset, NINTENDO_LOGO.size()));
}

auto Cartridge::readNoMBC(const word address) const -> byte {
    if (address >= MEMORY_FIXED_BANK_START && address <= MEMORY_SWITCH_BANK_END) {
        return address < rom.size() ? rom[address] : OPEN_BUS_VALUE;
    }

    if (address >= MEMORY_ERAM_START && address <= MEMORY_ERAM_END) {
        if (const auto offset = address & MASK_13_LOWER_WORD;
            offset < eRam.size()) {
            return eRam[offset];
        }
    }

    return OPEN_BUS_VALUE;
}

void Cartridge::writeNoMBC(const word address, const byte value) {
    if (address >= MEMORY_ERAM_START && address <= MEMORY_ERAM_END) {
        if (const std::size_t offset = address & MASK_13_LOWER_WORD;
            offset < eRam.size()) {
            eRam[offset] = value;
        }
    }
}

auto Cartridge::readMBC1(const word address) const -> byte {
    const std::size_t numberOfRomBanks = rom.size() / CARTRIDGE_ROM_BANK_SIZE;
    if (numberOfRomBanks == 0) {
        return OPEN_BUS_VALUE;
    }

    const byte upperBankShift = multicartEnabled ? 4 : 5;
    const byte lowerBankMask = multicartEnabled ? MASK_4_LOWER_BYTE : MASK_5_LOWER_BYTE;

    // Fixed ROM bank: 0x0000 - 0x3FFF.
    if (address <= MEMORY_FIXED_BANK_END) {
        const std::size_t romBank = bankingMode == CARTRIDGE_ERAM_BANKING_MODE ?
          static_cast<std::size_t>(eRamBankingNumber) << upperBankShift : 0;

        const std::size_t physicalBank = romBank % numberOfRomBanks;
        const std::size_t offset = physicalBank * CARTRIDGE_ROM_BANK_SIZE + address;
        return offset < rom.size() ? rom[offset] : OPEN_BUS_VALUE;
    }

    // Switchable ROM bank: 0x4000 - 0x7FFF.
    if (address >= MEMORY_SWITCH_BANK_START &&
        address <= MEMORY_SWITCH_BANK_END) {

      const bool bankZero = (romBankingNumber & MASK_5_LOWER_BYTE) == 0;
      std::size_t romBank = (romBankingNumber & lowerBankMask) | (static_cast<std::size_t>(eRamBankingNumber) << upperBankShift);

      if (bankZero) {
        romBank |= 0x01;
      }

      const std::size_t physicalBank = romBank % numberOfRomBanks;
      const std::size_t offset = physicalBank * CARTRIDGE_ROM_BANK_SIZE + (address - MEMORY_SWITCH_BANK_START);
      return offset < rom.size() ? rom[offset] : OPEN_BUS_VALUE;
    }

    // External RAM: 0xA000 - 0xBFFF.
    if (address >= MEMORY_ERAM_START && address <= MEMORY_ERAM_END && eRamEnabled && !eRam.empty()) {
        const std::size_t numberOfRamBanks = (eRam.size() + CARTRIDGE_ERAM_BANK_SIZE - 1) / CARTRIDGE_ERAM_BANK_SIZE;

        const std::size_t ramBank = bankingMode == CARTRIDGE_ERAM_BANKING_MODE ?
          eRamBankingNumber & MASK_2_LOWER_BYTE : 0;

        const std::size_t physicalBank = ramBank % numberOfRamBanks;
        std::size_t offset = physicalBank * CARTRIDGE_ERAM_BANK_SIZE + (address - MEMORY_ERAM_START);

        // Mirror RAM smaller than 8 KB.
        offset %= eRam.size();
        return eRam[offset];
    }

    return OPEN_BUS_VALUE;
}

void Cartridge::writeMBC1(const word address, const byte value) {
    // RAM enable: 0x0000 - 0x1FFF.
    if (address <= CARTRIDGE_ERAM_ENABLE_END) {
        eRamEnabled = (value & MASK_4_LOWER_BYTE) == CARTRIDGE_ERAM_ENABLE_VALUE;

    } else if (address >= CARTRIDGE_ROM_BANK_NUMBER_START &&
      address <= CARTRIDGE_ROM_BANK_NUMBER_END) {

        // Store the raw lower 5 bits.
        // Bank-zero remapping happens when reading ROM.
        romBankingNumber = value & MASK_5_LOWER_BYTE;

    } else if (address >= CARTRIDGE_ERAM_BANK_NUMBER_START &&
               address <= CARTRIDGE_ERAM_BANK_NUMBER_END) {

        // These bits are used for ROM banking in mode 0
        // and RAM banking / fixed ROM banking in mode 1.
        eRamBankingNumber = value & MASK_2_LOWER_BYTE;

    } else if (address >= CARTRIDGE_ROM_ERAM_MODE_SELECT_START &&
               address <= CARTRIDGE_ROM_ERAM_MODE_SELECT_END) {

        bankingMode = value & MASK_1_LOWER_BYTE;

    } else if (address >= MEMORY_ERAM_START && address <= MEMORY_ERAM_END &&
               eRamEnabled &&
               !eRam.empty()) {

        const std::size_t numberOfRamBanks = (eRam.size() + CARTRIDGE_ERAM_BANK_SIZE - 1) / CARTRIDGE_ERAM_BANK_SIZE;
        const std::size_t ramBank = bankingMode == CARTRIDGE_ERAM_BANKING_MODE ?
          eRamBankingNumber & MASK_2_LOWER_BYTE : 0;

        const std::size_t physicalBank = ramBank % numberOfRamBanks;
        std::size_t offset = physicalBank * CARTRIDGE_ERAM_BANK_SIZE + (address - MEMORY_ERAM_START);

        // Mirror RAM smaller than 8 KB.
        offset %= eRam.size();
        eRam[offset] = value;
    }
}

bool Cartridge::isMulticart() {
  if (mapper != Mapper::MBC1) {
    return false;
  }

  int nintendoLogoCount = 0;
  for (int i = 0; i < 4; ++i) {
    nintendoLogoCount += hasNintendoLogo(NINTENDO_LOGO_START + (CARTRIDGE_ROM_SIZE * i)) ? 1 : 0;
  }

  return nintendoLogoCount >= 2;
}

auto Cartridge::readMBC3(const word address) const -> byte {
    // Fixed ROM bank.
    if (address <= MEMORY_FIXED_BANK_END) {
        return address < rom.size() ? rom[address] : OPEN_BUS_VALUE;
    }

    // Switchable ROM bank.
    if (address >= MEMORY_SWITCH_BANK_START &&
        address <= MEMORY_SWITCH_BANK_END) {

        const std::size_t offset = static_cast<std::size_t>(romBankingNumber) * CARTRIDGE_ROM_BANK_SIZE +
          (address - MEMORY_SWITCH_BANK_START);
        return offset < rom.size() ? rom[offset] : OPEN_BUS_VALUE;
    }

    // External RAM or RTC.
    if (address >= MEMORY_ERAM_START &&
        address <= MEMORY_ERAM_END &&
        eRamEnabled) {

        switch (eRamBankingNumber) {
            case 0x0:
            case 0x1:
            case 0x2:
            case 0x3: {
                const std::size_t offset = static_cast<std::size_t>(eRamBankingNumber) * CARTRIDGE_ERAM_BANK_SIZE +
                    (address - MEMORY_ERAM_START);
                return offset < eRam.size() ? eRam[offset] : OPEN_BUS_VALUE;
            }
            case 0x8:
                return rtcSeconds;
            case 0x9:
                return rtcMinutes;
            case 0xA:
                return rtcHours;
            case 0xB:
                return rtcDaysLow;
            case 0xC:
                return rtcDaysHigh;
            default:
                return OPEN_BUS_VALUE;
        }
    }

    return OPEN_BUS_VALUE;
}

void Cartridge::writeMBC3(const word address, const byte value) {
    // RAM and RTC enable.
    if (address <= CARTRIDGE_ERAM_ENABLE_END) {
        eRamEnabled =
            (value & MASK_4_LOWER_BYTE) == CARTRIDGE_ERAM_ENABLE_VALUE;
    }else if (address >= CARTRIDGE_ROM_BANK_NUMBER_START &&
             address <= CARTRIDGE_ROM_BANK_NUMBER_END) {
        romBankingNumber = value & MASK_7_LOWER_BYTE;
        if (romBankingNumber == 0) {
            romBankingNumber = 1;
        }
    } else if (address >= CARTRIDGE_ERAM_BANK_NUMBER_START &&
               address <= CARTRIDGE_ERAM_BANK_NUMBER_END) {
        eRamBankingNumber = value & MASK_4_LOWER_BYTE;

    } else if (address >= CARTRIDGE_ROM_ERAM_MODE_SELECT_START &&
               address <= CARTRIDGE_ROM_ERAM_MODE_SELECT_END) {

        if (rtcLatch == 0 && value == 1) {
            updateRtcLatchMBC3();
        }
        rtcLatch = value;

    }else if (address >= MEMORY_ERAM_START &&
             address <= MEMORY_ERAM_END &&
             eRamEnabled) {

        switch (eRamBankingNumber) {
            case 0x0:
            case 0x1:
            case 0x2:
            case 0x3: {
                const std::size_t offset = static_cast<std::size_t>(eRamBankingNumber) * CARTRIDGE_ERAM_BANK_SIZE +
                    (address - MEMORY_ERAM_START);

                if (offset < eRam.size()) {
                    eRam[offset] = value;
                }
                break;
            }
            case 0x8:
                rtcSeconds = value & MASK_6_LOWER_BYTE;
                break;
            case 0x9:
                rtcMinutes = value & MASK_6_LOWER_BYTE;
                break;
            case 0xA:
                rtcHours = value & MASK_5_LOWER_BYTE;
                break;
            case 0xB:
                rtcDaysLow = value;
                break;
            case 0xC:
                rtcDaysHigh = value;
                break;
            default:
                break;
        }
    }
}

void Cartridge::updateRtcLatchMBC3() {
    // Uses time since epoch. Replace later with an accurate RTC counter.
    const auto now =
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();

    rtcSeconds = static_cast<byte>(now % 60);
    rtcMinutes = static_cast<byte>(now / 60 % 60);
    rtcHours = static_cast<byte>(now / 3600 % 24);

    const word days = static_cast<word>(now / 86400);

    rtcDaysLow = static_cast<byte>(days & MASK_8_LOWER_WORD);

    rtcDaysHigh = rtcDaysHigh & MASK_8_UPPER_BYTE |
      static_cast<byte>(days >> 8 & MASK_1_LOWER_WORD);

    if (days > 511) {
        rtcDaysHigh |= MASK_1_UPPER_BYTE;
    }
}

auto Cartridge::readMBC5(const word address) const -> byte {
  const std::size_t numberOfRomBanks = rom.size() / CARTRIDGE_ROM_BANK_SIZE;
  if (numberOfRomBanks == 0) {
    return OPEN_BUS_VALUE;
  }

  // Fixed ROM bank.
  if (address <= MEMORY_FIXED_BANK_END) {
    return address < rom.size() ? rom[address] : OPEN_BUS_VALUE;
  }

  // Switchable ROM bank.
  if (address >= MEMORY_SWITCH_BANK_START &&
      address <= MEMORY_SWITCH_BANK_END) {
      const std::size_t physicalBank = romBankingNumber % numberOfRomBanks;
      const std::size_t offset = physicalBank * CARTRIDGE_ROM_BANK_SIZE +
          (address - MEMORY_SWITCH_BANK_START);

      return offset < rom.size() ? rom[offset] : OPEN_BUS_VALUE;
    }

  // External RAM.
  if (address >= MEMORY_ERAM_START &&
      address <= MEMORY_ERAM_END &&
      eRamEnabled &&
      !eRam.empty()) {

    const std::size_t numberOfRamBanks =
        (eRam.size() + CARTRIDGE_ERAM_BANK_SIZE - 1) / CARTRIDGE_ERAM_BANK_SIZE;

    const std::size_t physicalBank = eRamBankingNumber % numberOfRamBanks;
    const std::size_t offset = physicalBank * CARTRIDGE_ERAM_BANK_SIZE +
        (address - MEMORY_ERAM_START);

    return offset < eRam.size() ? eRam[offset] : OPEN_BUS_VALUE;
  }

  return OPEN_BUS_VALUE;
}

void Cartridge::writeMBC5(const word address, const byte value) {
  // RAM enable.
  if (address <= CARTRIDGE_ERAM_ENABLE_END) {
    eRamEnabled = (value & MASK_4_LOWER_BYTE) == CARTRIDGE_ERAM_ENABLE_VALUE;

    // ROM bank bits 0-7.
  } else if (address >= CARTRIDGE_ROM_BANK_NUMBER_START && address <= CARTRIDGE_ROM_BANK_NUMBER_LOWER_END) {
    romBankingNumber = (romBankingNumber & MASK_9_WORD) | value;

    // ROM bank bit 8.
  } else if (address >= CARTRIDGE_ROM_BANK_NUMBER_UPPER_START && address <= CARTRIDGE_ROM_BANK_NUMBER_END) {
    romBankingNumber = (romBankingNumber & MASK_8_LOWER_WORD) |
        (static_cast<word>(value & MASK_1_LOWER_BYTE) << 8);

    // RAM bank bits 0-3.
  } else if (address >= CARTRIDGE_ERAM_BANK_NUMBER_START &&
             address <= CARTRIDGE_ERAM_BANK_NUMBER_END) {
    eRamBankingNumber = value & MASK_4_LOWER_BYTE;

    // External RAM.
   } else if (address >= MEMORY_ERAM_START &&
              address <= MEMORY_ERAM_END &&
              eRamEnabled &&
              !eRam.empty()) {

      const std::size_t numberOfRamBanks =
         (eRam.size() + CARTRIDGE_ERAM_BANK_SIZE - 1) / CARTRIDGE_ERAM_BANK_SIZE;

      const std::size_t physicalBank = eRamBankingNumber % numberOfRamBanks;
      const std::size_t offset = physicalBank * CARTRIDGE_ERAM_BANK_SIZE +
         (address - MEMORY_ERAM_START);

      if (offset < eRam.size()) {
       eRam[offset] = value;
      }
    }
}
}  // namespace gbdmge