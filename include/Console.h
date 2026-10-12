#ifndef GBDMGE_CONSOLE_H
#define GBDMGE_CONSOLE_H

#include "Bus.h"
#include "Cartridge.h"
#include "CPU.h"

namespace gbdmge {
class Console {
 public:
  Console();

  auto load(const std::filesystem::path& romPath) -> bool;
  void step();

 private:
  Bus bus;
  CPU cpu;
  Cartridge cartridge;
  friend class Bus;
};
}  // namespace gbdmge

#endif