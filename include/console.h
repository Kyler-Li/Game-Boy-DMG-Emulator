#ifndef GBDMGE_CONSOLE_H
#define GBDMGE_CONSOLE_H

#include "bus.h"
#include "cartridge.h"
#include "cpu.h"

namespace gbdmge {
class Console {
 public:
  Console();

  auto load(const std::filesystem::path& romPath) -> bool;
  void play();

 private:
  Bus bus;
  CPU cpu;
  Cartridge cartridge;
  friend class Bus;
};
}  // namespace gbdmge

#endif