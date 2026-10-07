#ifndef GBDMGE_CPU_H
#define GBDMGE_CPU_H

#include "Types.h"

namespace gbdmge {
class Bus;

class CPU {
 public:
  explicit CPU(Bus& bus);
  byte fetchByte();
  word fetchWord();

  // NOTE: Don't forget to account for cycles to writes to
  // memory mapped registers. For example, writes to initiate a
  // DMA
  qword execute(byte opcode);
  void reset();

 private:
  bool ime;
  bool isHalted;
  Bus* bus;

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

  // IMPORTANT FOR ALL INSTRUCTIONS:
  // Instruction implementation functions ASSUME they have received control
  // after their opcode has been fetched, and PC points to the first operand byte. That is they
  // operate after a call to fetchByte().
  //
  // For example, the call instruction assumes the PC points the first byte of the address
  // it will jump to (one byte beyond its opcode).
  //                            |Address | Value  |  Meaning                  |
  //                            | 0x100  |  0xCD  |  Opcode for call          |
  //  PC when call executes ->  | 0x101  |  0xAB  |  Low byte address         |
  //                            | 0x102  |  0xCD  |  High byte address        |
  //                            | 0x103  |   ??   |  Next instruction opcode  |
  void pop(byte& high, byte& low);
  void push(byte high, byte low);
  void andA(byte src);
  void orA(byte src);
  void xorA(byte src);
  void cp(byte cmp);
  void cp(word address);
  void jpZ(word address);
  void jpNz(word address);
  void jpC(word address);
  void jpNc(word address);
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
  void retI();
  void retC();
  void retZ();
  void retNc();
  void retNz();
  void retCond(bool cond);
  void ret();
  void halt();
  void sbc(byte src);
  void sbc(word address);
  void scf();
  void ccf();
  void rlca();
  void callC();
  void callNc();
  void callZ();
  void callNz();
  void callCond(bool cond);
  void call();
  void ld(byte& dst, byte src);
  void ld(word address, byte src);
  void ld(byte& dst, word address);
  void ld(byte& high, byte& low, word value);
  void inc(byte& high, byte& low);
  void inc(word address);
  void inc(byte& dst);
  void dec(byte& high, byte& low);
  void dec(word address);
  void dec(byte& dst);
};
}  // namespace gbdmge

#endif
