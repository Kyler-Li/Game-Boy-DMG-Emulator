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

  // Stack instructions
  void pop(byte& high, byte& low);
  void push(byte high, byte low);

  // Logical instructions
  void andA(word address);
  void andA(byte src);
  void orA(word address);
  void orA(byte src);
  void xorA(word address);
  void xorA(byte src);
  void cpl();

  // Comparisons
  void cp(byte cmp);
  void cp(word address);

  // Jumps
  bool jpZ(word address);
  bool jpNz(word address);
  bool jpC(word address);
  bool jpNc(word address);
  void jp(word address);
  bool jpCond(word address, bool cond);
  bool jrC(sbyte src);
  bool jrNc(sbyte src);
  bool jrZ(sbyte src);
  bool jrNz(sbyte src);
  void jr(sbyte src);
  bool jrCond(sbyte src, bool cond);
  void rst(word address);

  void nop();
  void ei();
  void di();

  // Returns
  bool retI();
  bool retC();
  bool retZ();
  bool retNc();
  bool retNz();
  void ret();
  bool retCond(bool cond);

  void halt();
  void scf();
  void ccf();

  // Calls
  bool callC();
  bool callNc();
  bool callZ();
  bool callNz();
  void call();
  bool callCond(bool cond);

  // Loads
  void ld(byte& dst, byte src);
  void ld(word address, byte src);
  void ld(byte& dst, word address);
  void ld(byte& high, byte& low, word value);
  void ldSp(word address);
  void ldSp();
  void ldSpHl();
  void ldHlSp(sbyte src);

  // BCD
  void daa();

  // Arithmetic
  void inc(byte& high, byte& low);
  void inc(word address);
  void inc(byte& dst);
  void dec(byte& high, byte& low);
  void dec(word address);
  void dec(byte& dst);
  void decSp();
  void add(byte& dst, word address, bool useCarry = false);
  void add(byte& dst, byte src, bool useCarry = false);
  void addHl(byte high, byte low);
  void addSp(sbyte src);
  void sub(word address, bool useCarry = false);
  void sub(byte src, bool useCarry = false);

  // Shifts
  void rlca();
  void rrca();
  void rla();
  void rra();
  void rlc(byte& dst);
  void rrc(byte& dst);
  void rl(byte& dst);
  void rr(byte& dst);
  void sla(byte& dst);
  void sra(byte& dst);
  void srl(byte& dst);
  void shift(byte& dst, bool throughCarry, bool shiftLeft);
  void shiftCb(byte& dst, bool preserveSign, bool shiftLeft);

  // Bitwise instructions
  void set(byte& dst, byte pos,
           bool set);  // For both set and res instructions
  void swap(byte& dst);
  void bit(word address, byte pos);
  void bit(byte src, byte pos);

  qword executeCb(byte opcode);
};
}  // namespace gbdmge

#endif
