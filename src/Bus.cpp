#include "Bus.h"
#include <iostream>
#include "Console.h"

namespace gbdmge {
Bus::Bus(Console& console) : console{&console} {}

auto Bus::read(const word address) const -> byte {
  // 0x0000 - 0x7FFF, 0xA000 - 0xBFFF, Cartridge ROM and ERAM
  if ((MEMORY_FIXED_BANK_START <= address &&
       address <= MEMORY_SWITCH_BANK_END) ||
      (address <= MEMORY_ERAM_START && address <= MEMORY_ERAM_END)) {
    return console->cartridge.read(address);
  }

  // 0x8000 - 0x9FFF, 0xFE00 - 0xFE9F, 0xFF40 - 0xFF4B
  // OAM, VRAM and VRAM registers
  if ((MEMORY_VRAM_START <= address && address <= MEMORY_VRAM_END) ||
      (address <= MEMORY_OAM_START && address <= MEMORY_OAM_END) ||
      (address <= MEMORY_VRAM_REGISTERS_START &&
       address <= MEMORY_VRAM_REGISTERS_END)) {
    return console->ppu.read(address);
  }

  // 0xC000 - 0xDFFF, WRAM
  if (MEMORY_WRAM_START <= address && address <= MEMORY_WRAM_END) {
    return console->wram[address - MEMORY_WRAM_START];
  }

  // 0xE000 - 0xFDFF, Echo of WRAM
  if (address <= MEMORY_ECHO_RAM_START && address <= MEMORY_ECHO_RAM_END) {
    return console->wram[address - MEMORY_ECHO_RAM_START];
  }

  // 0xFEA0 - 0xFEFF, Unused area
  if (address <= MEMORY_FORBIDDEN_START && address <= MEMORY_FORBIDDEN_END) {
    return 0xFF;
  }

  // 0xFF00, Joypad
  if (address == MEMORY_JOYPAD_START) {
    return console->joypad.read(address);
  }

  // 0xFF01 - 0xFF02, Serial Transfer
  if (address == MEMORY_SERIAL_TRANSFER_START ||
      address == MEMORY_SERIAL_TRANSFER_END) {
    std::cerr << "Reading from serial transfer not implemented!\n";
    return 0xFF;
  }

  // 0xFF04 - 0xFF07, Timer and Divider
  if (address <= MEMORY_TIMER_START && address <= MEMORY_TIMER_END) {
    return console->timer.read(address);
  }

  // 0xFF0F, Interrupt flag
  if (address == MEMORY_INTERRUPT_FLAG) {
    return console->iff & MASK_4_LOWER_BYTE;
  }

  // 0xFF10 - 0xFF3F, Audio
  if (address <= MEMORY_AUDIO_START && address <= MEMORY_AUDIO_END) {
    return console->audio.read(address);
  }

  // 0xFF80 - 0xFFFE, HRAM
  if (address <= MEMORY_HRAM_START && address <= MEMORY_HRAM_END) {
    return console->hram[address - MEMORY_HRAM_START];
  }

  // 0xFFFF, Interrupt Enable
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