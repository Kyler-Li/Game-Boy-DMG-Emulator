#ifndef GBDMGE_CPU_H
#define GBDMGE_CPU_H

#include "register.h"
#include "types.h"

namespace gbdmge {
class Bus;

class CPU {
 public:
  explicit CPU(Bus& bus);
  byte fetch();

  // NOTE: Don't forget to account for cycles to writes to
  // memory mapped registers. For example, writes to initiate a
  // DMA
  qword execute(byte opcode);
  void reset();

 private:
  bool ime;
  Bus* bus{};

  // For each Reg, the rightmost letter represents low byte,
  // leftmost letter represents high byte. For example:
  // high byte    | low byte
  // a            | f
  // b            | c
  // d            | e
  // ...
  Register af;
  Register bc;
  Register de;
  Register hl;
  Register sp;
  Register pc;

  qword tCycles;  // 1 T-cycle = 1 clock cycle, 1 M-cycle = 4 clock cycles = 4
                  // T-cycles

  void pop(Register& src);
  void push(Register& src);
  void anda(byte src);
  void ora(byte src);
  void xora(byte src);
  void cp(byte b);
  void cp(Register& b);
  void jpz(word address);
  void jpnz(word address);
  void jpc(word address);
  void jpnc(word address);
  void jp(Register& dst);
  void jp(word address);
  void jpCond(word address, bool cond);
  void nop();
  void cpl();
  void ei();
  void di();
  void jrC(byte src);
  void jrNc(byte src);
  void jrZ(byte src);
  void jrNz(byte src);
  void jr(byte src);
  void jrCond(byte src, bool cond);
};
}  // namespace gbdmge

#endif
