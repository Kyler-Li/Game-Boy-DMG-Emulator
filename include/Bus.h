#ifndef GBDMGE_BUS_H
#define GBDMGE_BUS_H

#include <array>
#include "Constants.h"
#include "Types.h"

namespace gbdmge {
class Console;

//! @brief Intercepts external read and write requests and reroutes them across
//! different components. This allows different components to respond accordingly
//! for requests targeted for their respective memory ranges.
class Bus {
 public:
  Bus(Console& console);
  auto read(word address) -> byte;
  void write(word address, byte value);

 private:
  Console* console{};

  // TODO: Some memory is currently being stored in the bus, but eventually they should be moved to their respective components
  // E.g., VRAM should be moved into the PPU once we have a class for it
  // Though some, like wram, may not have a component associated with it, those can stay in the bus
  std::array<byte, MEMORY_VRAM_END - MEMORY_VRAM_START + 1> vram;
  std::array<byte, MEMORY_WRAM_END - MEMORY_WRAM_START + 1> wram;
  std::array<byte, MEMORY_SWITCH_WRAM_END - MEMORY_SWITCH_WRAM_START + 1>
      switchableWram;
};
}  // namespace gbdmge

#endif