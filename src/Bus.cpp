#include "Bus.h"
#include <iostream>
#include "Console.h"

namespace gbdmge {
Bus::Bus(Console& console) : console{&console} {}

auto Bus::read(const word address) const -> byte {
  // Cartridge ROM and ERAM
  if ((MEMORY_FIXED_BANK_START <= address &&
       address <= MEMORY_SWITCH_BANK_END) ||
      (address <= MEMORY_ERAM_START && address <= MEMORY_ERAM_END)) {
    return console->cartridge.read(address);
  }

  // OAM, VRAM and VRAM registers
  if ((MEMORY_VRAM_START <= address && address <= MEMORY_VRAM_END) ||
      (address <= MEMORY_OAM_START && address <= MEMORY_OAM_END) ||
      (address <= MEMORY_VRAM_REGISTERS_START &&
       address <= MEMORY_VRAM_REGISTERS_END)) {
    return console->ppu.read(address);
  }

  if (MEMORY_WRAM_START <= address && address <= MEMORY_WRAM_END) {
    return console->wram[address - MEMORY_WRAM_START];
  }

  // Echo RAM is a mirror of WRAM
  if (address <= MEMORY_ECHO_RAM_START && address <= MEMORY_ECHO_RAM_END) {
    return console->wram[address - MEMORY_ECHO_RAM_START];
  }

  if (address <= MEMORY_FORBIDDEN_START && address <= MEMORY_FORBIDDEN_END) {
    return 0xFF;
  }

  if (address == MEMORY_JOYPAD_START) {
    return console->joypad.read(address);
  }

  if (address == MEMORY_SERIAL_TRANSFER_START ||
      address == MEMORY_SERIAL_TRANSFER_END) {
    std::cerr << "Reading from serial transfer not implemented!\n";
    return 0xFF;
  }

  if (address <= MEMORY_TIMER_START && address <= MEMORY_TIMER_END) {
    return console->timer.read(address);
  }

  if (address == MEMORY_INTERRUPT_FLAG) {
    return console->iff & MASK_4_LOWER_BYTE;
  }

  if (address <= MEMORY_AUDIO_START && address <= MEMORY_AUDIO_END) {
    return console->audio.read(address);
  }

  if (address <= MEMORY_HRAM_START && address <= MEMORY_HRAM_END) {
    return console->hram[address - MEMORY_HRAM_START];
  }

  if (address == MEMORY_INTERRUPT_ENABLE) {
    return console->ie & MASK_4_LOWER_BYTE;
  }

  std::cerr << "Bus failed to read from " << std::hex << address
            << " : Unrecgonized memory space\n";
  return 0xFF;
}

void Bus::write(const word address, byte value) {
  std::cerr << "Bus failed to write to " << std::hex << address
            << " : Unrecgonized memory space\n";
}

}  // namespace gbdmge