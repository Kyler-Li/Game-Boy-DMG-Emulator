#ifndef GBDMGE_CONSOLE_H
#define GBDMGE_CONSOLE_H

#include "cpu.h"
#include "memory.h"

namespace gbdmge {
class Console {
 public:
  Console();

  void load(const std::filesystem::path& romPath);
  void play();

 private:
  CPU m_cpu;
  Memory m_memory;
  friend class CPU;
  friend class Memory;
};
}  // namespace gbdmge

#endif