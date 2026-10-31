#include "Bus.h"
#include <iostream>
#include "Console.h"

namespace gbdmge {
Bus::Bus(Console& console) : console{&console} {}

auto Bus::read(const word address) const -> byte {
  if ((address >= MEMORY_FIXED_BANK_START &&
       address <= MEMORY_SWITCH_BANK_END) ||
      (address >= MEMORY_ERAM_START && address <= MEMORY_ERAM_END)) {
    return console->cartridge.read(address);
  }

  if ((address >= MEMORY_VRAM_START && address <= MEMORY_VRAM_END) ||
      (address >= MEMORY_OAM_START && address <= MEMORY_OAM_END) ||
      (address >= MEMORY_LCD_START && address <= MEMORY_LCD_END)) {
    return console->ppu.read(address);
  }

  if (address >= MEMORY_WRAM_START && address <= MEMORY_WRAM_END) {
    return console->wram[address - MEMORY_WRAM_START];
  }

  // Echo RAM is a mirror of WRAM
  if (address >= MEMORY_ECHO_RAM_START && address <= MEMORY_ECHO_RAM_END) {
    return console->wram[address - MEMORY_ECHO_RAM_START];
  }

  if (address >= MEMORY_FORBIDDEN_START && address <= MEMORY_FORBIDDEN_END) {
    std::cerr << "Ignoring read from forbidden memory space.\n";
    return 0xFF;
  }

  if (address == MEMORY_P1_JOYP_ADDRESS) {
    return console->joypad.read(address);
  }

  if (address == MEMORY_SB_ADDRESS || address == MEMORY_SC_ADDRESS) {
    std::cerr << "Reading from serial transfer not implemented!\n";
    return 0xFF;
  }

  if (address >= MEMORY_TIMER_DIV_START && address <= MEMORY_TIMER_DIV_END) {
    return console->timer.read(address);
  }

  if (address == MEMORY_IF_ADDRESS) {
    return console->iff | MASK_3_UPPER_BYTE;
  }

  if (address >= MEMORY_AUDIO_START && address <= MEMORY_AUDIO_END) {
    return console->audio.read(address);
  }

  if (address >= MEMORY_HRAM_START && address <= MEMORY_HRAM_END) {
    return console->hram[address - MEMORY_HRAM_START];
  }

  if (address == MEMORY_IE_ADDRESS) {
    return console->ie | MASK_3_UPPER_BYTE;
  }

  std::cerr << "Bus failed to read from " << std::hex << address
            << " : Unrecgonized memory space\n";
  return 0xFF;
}

void Bus::write(const word address, const byte value) {
  if ((address >= MEMORY_FIXED_BANK_START &&
       address <= MEMORY_SWITCH_BANK_END) ||
      (address >= MEMORY_ERAM_START && address <= MEMORY_ERAM_END)) {
    console->cartridge.write(address, value);
    return;
  }

  if ((address >= MEMORY_VRAM_START && address <= MEMORY_VRAM_END) ||
      (address >= MEMORY_OAM_START && address <= MEMORY_OAM_END) ||
      (address >= MEMORY_LCD_START && address <= MEMORY_LCD_END)) {
    console->ppu.write(address, value);
    return;
  }

  if (address >= MEMORY_WRAM_START && address <= MEMORY_WRAM_END) {
    console->wram[address - MEMORY_WRAM_START] = value;
    return;
  }

  // Echo RAM is a mirror of WRAM
  if (address >= MEMORY_ECHO_RAM_START && address <= MEMORY_ECHO_RAM_END) {
    console->wram[address - MEMORY_ECHO_RAM_START] = value;
    return;
  }

  if (address >= MEMORY_FORBIDDEN_START && address <= MEMORY_FORBIDDEN_END) {
    std::cerr << "Ignoring write to forbidden memory space.\n";
    return;
  }

  if (address == MEMORY_P1_JOYP_ADDRESS) {
    console->joypad.write(address, value);
    return;
  }

  if (address == MEMORY_SB_ADDRESS || address == MEMORY_SC_ADDRESS) {
    std::cerr << "Reading from serial transfer not implemented!\n";
    return;
  }

  if (address >= MEMORY_TIMER_DIV_START && address <= MEMORY_TIMER_DIV_END) {
    console->timer.write(address, value);
    return;
  }

  if (address == MEMORY_IF_ADDRESS) {
    console->iff = value & MASK_5_LOWER_BYTE;
    return;
  }

  if (address >= MEMORY_AUDIO_START && address <= MEMORY_AUDIO_END) {
    console->audio.write(address, value);
    return;
  }

  if (address == MEMORY_DMA_ADDRESS) {
    // TODO: Processing should be handled within PPU and not Bus
    // Bus should only be responsible for forwarding requests to and from components
    const word srcBase = static_cast<word>(value) << 8;
    for (word dest = MEMORY_OAM_START; dest <= MEMORY_OAM_END; ++dest) {
      const word src = srcBase + dest - MEMORY_OAM_START;
      console->ppu.write(dest, read(src));
    }
    return;
  }

  if (address >= MEMORY_HRAM_START && address <= MEMORY_HRAM_END) {
    console->hram[address - MEMORY_HRAM_START] = value;
    return;
  }

  if (address == MEMORY_IE_ADDRESS) {
    console->ie = value & MASK_5_LOWER_BYTE;
    return;
  }

  std::cerr << "Bus failed to write to " << std::hex << address
            << " : Unrecgonized memory space\n";
}

}  // namespace gbdmge
