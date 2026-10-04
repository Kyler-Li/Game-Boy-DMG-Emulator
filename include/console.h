#ifndef GBDMGE_CONSOLE_H
#define GBDMGE_CONSOLE_H

#include "cartridge.h"
#include "cpu.h"
#include "memory.h"

namespace gbdmge {
class Console {
 public:
  Console();

  auto load(const std::filesystem::path& romPath) -> bool;
  void play();

 private:
  CPU cpu;
  Memory memory;
  Cartridge cartridge;
  friend class CPU;
  friend class Cartridge;
  friend class Memory;
};
}  // namespace gbdmge

#endif