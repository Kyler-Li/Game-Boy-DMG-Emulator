#ifndef GBDMGE_CPU_H
#define GBDMGE_CPU_H

#include "Types.h"

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
  bool isHalted;
  Bus* bus{};

  // For each register, the rightmost letter represents low byte,
  // leftmost letter represents high byte. For example:
  // high byte    | low byte
  // a            | f
  // b            | c
  // d            | e
  // h            | l
  byte a, f;
  byte b, c;
  byte d, e;
  byte h, l;

  word sp;
  word pc;

  qword tCycles;  // 1 T-cycle = 1 clock cycle, 1 M-cycle = 4 clock cycles = 4
                  // T-cycles

  void pop(byte& high, byte& low);
  void push(byte high, byte low);
  void anda(byte src);
  void ora(byte src);
  void xora(byte src);
  void cp(byte cmp);
  void cp(byte high, byte low);
  void jpz(word address);
  void jpnz(word address);
  void jpc(word address);
  void jpnc(word address);
  void jp(byte high, byte low);
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
  void reti();
  void retc();
  void retz();
  void retnc();
  void retnz();
  void retCond(bool cond);
  void ret();
  void halt();
  void sbc(byte src);
  void sbc(byte high, byte low);
  void rlca();
};
}  // namespace gbdmge

#endif
