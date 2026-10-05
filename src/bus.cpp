#include "bus.h"
#include <iostream>
#include "console.h"

namespace gbdmge {
Bus::Bus(Console& console) : console{&console} {}

auto Bus::read(word address) -> byte {
  if ((MEMORY_FIXED_BANK_START <= address && MEMORY_SWITCH_BANK_END) ||
      (MEMORY_ERAM_START <= address && address <= MEMORY_ERAM_END)) {
    return console->cartridge.read(address);
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

  std::cerr << "Bus failed to write to " << std::hex << address
            << " : Unrecgonized memory space\n";
}

}  // namespace gbdmge