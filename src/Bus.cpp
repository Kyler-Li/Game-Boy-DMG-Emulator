#include "Bus.h"
#include <iostream>
#include "Console.h"

namespace gbdmge {
Bus::Bus(Console& console) : console{&console} {}

auto Bus::read(word address) -> byte {
  if ((MEMORY_FIXED_BANK_START <= address && MEMORY_SWITCH_BANK_END) ||
      (MEMORY_ERAM_START <= address && address <= MEMORY_ERAM_END)) {
    return console->cartridge.read(address);
  }

  if (MEMORY_VRAM_START <= address && address <= MEMORY_VRAM_END) {
    return console->ppu.read(address);
  }

  if (address == MEMORY_IO_JOYPAD_INPUT) {
    return console->joypad.read(address);
  }

  // Unimplemented
  if (address == MEMORY_IO_SERIAL_TRANSFER_DATA ||
      address == MEMORY_IO_SERIAL_TRANSFER_CONTROL) {
    return 0xFF;
  }

  if (MEMORY_IO_AUDIO_START <= address && address <= MEMORY_IO_AUDIO_END) {
    return console->audio.read(address);
  }

  std::cerr << "Bus failed to read from " << std::hex << address
            << " : Unrecgonized memory space\n";
  return 0xFF;
}

void Bus::write(word address, byte value) {
  if ((MEMORY_FIXED_BANK_START <= address && MEMORY_SWITCH_BANK_END) ||
      (MEMORY_ERAM_START <= address && address <= MEMORY_ERAM_END)) {
    console->cartridge.write(address, value);
    return;
  }

  if (MEMORY_VRAM_START <= address && address <= MEMORY_VRAM_END) {
    console->ppu.write(address, value);
    return;
  }

  if (address == MEMORY_IO_JOYPAD_INPUT) {
    console->joypad.write(address, value);
    return;
  }

  // Unimplemented
  if (address == MEMORY_IO_SERIAL_TRANSFER_DATA ||
      address == MEMORY_IO_SERIAL_TRANSFER_CONTROL) {
    return;
  }

  if (MEMORY_IO_AUDIO_START <= address && address <= MEMORY_IO_AUDIO_END) {
    console->audio.write(address, value);
    return;
  }

  std::cerr << "Bus failed to write to " << std::hex << address
            << " : Unrecgonized memory space\n";
}

}  // namespace gbdmge