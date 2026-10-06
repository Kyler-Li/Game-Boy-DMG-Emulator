#include "CPU.h"
#include "BitOps.h"
#include "Bus.h"
#include "Constants.h"

#include <stdexcept>

namespace gbdmge {
CPU::CPU(Bus& bus)
    : ime{false},
      isHalted{false},
      bus{&bus},
      a{0},
      f{0},
      b{0},
      c{0},
      d{0},
      e{0},
      h{0},
      l{0},
      sp{0},
      pc{0},
      tCycles{0} {}

void CPU::pop(byte& high, byte& low) {
  const word address = sp;
  high = bus->read(address + 1);
  low = bus->read(address);
  sp += 2;
}

void CPU::push(const byte high, const byte low) {
  const word address = sp;
  bus->write(address - 1, high);
  bus->write(address - 2, low);
  sp -= 2;
}

void CPU::anda(const byte src) {
  a = a & src;
  f = 0;
  setBit(f, CPU_ZERO_FLAG, a == 0);
  setBit(f, CPU_HALF_CARRY_FLAG, true);
}

void CPU::ora(const byte src) {
  a = a | src;
  f = 0;
  setBit(f, CPU_ZERO_FLAG, a == 0);
}

void CPU::xora(const byte src) {
  a = a ^ src;
  f = 0;
  setBit(f, CPU_ZERO_FLAG, a == 0);
}

void CPU::cp(const byte cmp) {
  setBit(f, CPU_ZERO_FLAG, a == cmp);
  setBit(f, CPU_SUB_FLAG, true);
  setBit(f, CPU_HALF_CARRY_FLAG,
         (a & MASK_4_LOWER_BYTE) < (cmp & MASK_4_LOWER_BYTE));
  setBit(f, CPU_CARRY_FLAG, a < cmp);
}

void CPU::cp(const byte high, const byte low) {
  cp(bus->read(makeWord(high, low)));
}

void CPU::jpz(const word address) {
  jpCond(address, bitIsSet(f, CPU_ZERO_FLAG));
}

void CPU::jpnz(const word address) {
  jpCond(address, !bitIsSet(f, CPU_ZERO_FLAG));
}

void CPU::jpc(const word address) {
  jpCond(address, bitIsSet(f, CPU_CARRY_FLAG));
}

void CPU::jpnc(const word address) {
  jpCond(address, !bitIsSet(f, CPU_CARRY_FLAG));
}

void CPU::jp(const byte high, const byte low) {
  jpCond(makeWord(high, low), true);
}

void CPU::jp(const word address) {
  jpCond(address, true);
}

void CPU::jpCond(const word address, const bool cond) {
  if (cond) {
    pc = address;
  }
}

void CPU::nop() {}

void CPU::cpl() {
  a = ~a;
  setBit(f, CPU_HALF_CARRY_FLAG, true);
  setBit(f, CPU_SUB_FLAG, true);
}

void CPU::ei() {
  ime = true;
}

void CPU::di() {
  ime = false;
}

void CPU::jrC(const byte src) {
  jrCond(src, bitIsSet(f, CPU_CARRY_FLAG));
}

void CPU::jrNc(const byte src) {
  jrCond(src, !bitIsSet(f, CPU_CARRY_FLAG));
}

void CPU::jrZ(const byte src) {
  jrCond(src, bitIsSet(f, CPU_ZERO_FLAG));
}

void CPU::jrNz(const byte src) {
  jrCond(src, !bitIsSet(f, CPU_ZERO_FLAG));
}

void CPU::jr(const byte src) {
  jrCond(src, true);
}

void CPU::jrCond(const byte src, const bool cond) {
  if (cond) {
    pc += static_cast<sbyte>(src);
  }
}

void CPU::reti() {
  ime = true;
  ret();
}

void CPU::retc() {
  retCond(bitIsSet(f, CPU_CARRY_FLAG));
}

void CPU::retz() {
  retCond(bitIsSet(f, CPU_ZERO_FLAG));
}

void CPU::retnc() {
  retCond(!bitIsSet(f, CPU_CARRY_FLAG));
}

void CPU::retnz() {
  retCond(!bitIsSet(f, CPU_ZERO_FLAG));
}

void CPU::retCond(const bool cond) {
  if (cond) {
    ret();
  }
}

void CPU::ret() {
  pc = makeWord(bus->read(sp + 1), bus->read(sp));
  sp += 2;
}

void CPU::halt() {
  isHalted = true;
}

void CPU::sbc(const byte src) {
  const byte carry = getBit(f, CPU_CARRY_FLAG);
  const byte result = a - src - carry;

  setBit(f, CPU_ZERO_FLAG, result == 0);
  setBit(f, CPU_SUB_FLAG, true);
  setBit(f, CPU_HALF_CARRY_FLAG,
         (a & MASK_4_LOWER_BYTE) < (src & MASK_4_LOWER_BYTE) + carry);
  setBit(f, CPU_CARRY_FLAG, a < src + carry);

  a = result;
}

void CPU::sbc(const byte high, const byte low) {
  sbc(bus->read(makeWord(high, low)));
}

void CPU::rlca() {
  f = 0;
  const byte highBit = getBit(a, BIT_7);
  a <<= 1;
  a |= highBit;
  setBit(f, CPU_CARRY_FLAG, highBit);
}

void CPU::reset() {
  // Initialize to post boot ROM values
  ime = false;
  isHalted = false;

  a = 0x01;
  f = 0xB0;
  b = 0x00;
  c = 0x13;
  d = 0x00;
  e = 0xD8;
  h = 0x01;
  l = 0x4D;

  sp = 0xFFFE;
  pc = 0x0100;
  // Potentially reset T-cycles here
}

byte CPU::fetch() {
  const word address = pc;
  ++pc;
  return bus->read(address);
}

// Returns the number of T-cycles after execution
qword CPU::execute(byte opcode) {
  throw std::logic_error("not implemented");
}
}  // namespace gbdmge
