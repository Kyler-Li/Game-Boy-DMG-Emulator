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

void CPU::andA(const byte src) {
  a = a & src;
  f = 0;
  setBit(f, CPU_ZERO_FLAG, a == 0);
  setBit(f, CPU_HALF_CARRY_FLAG, true);
}

void CPU::orA(const byte src) {
  a = a | src;
  f = 0;
  setBit(f, CPU_ZERO_FLAG, a == 0);
}

void CPU::xorA(const byte src) {
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

void CPU::cp(const word address) {
  cp(bus->read(address));
}

void CPU::jpZ(const word address) {
  jpCond(address, bitIsSet(f, CPU_ZERO_FLAG));
}

void CPU::jpNz(const word address) {
  jpCond(address, !bitIsSet(f, CPU_ZERO_FLAG));
}

void CPU::jpC(const word address) {
  jpCond(address, bitIsSet(f, CPU_CARRY_FLAG));
}

void CPU::jpNc(const word address) {
  jpCond(address, !bitIsSet(f, CPU_CARRY_FLAG));
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
  setBit(f, CPU_SUB_FLAG, true);
  setBit(f, CPU_HALF_CARRY_FLAG, true);
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

void CPU::retI() {
  ime = true;
  ret();
}

void CPU::retC() {
  retCond(bitIsSet(f, CPU_CARRY_FLAG));
}

void CPU::retZ() {
  retCond(bitIsSet(f, CPU_ZERO_FLAG));
}

void CPU::retNc() {
  retCond(!bitIsSet(f, CPU_CARRY_FLAG));
}

void CPU::retNz() {
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
  setBit(
      f, CPU_CARRY_FLAG,
      static_cast<word>(src) + static_cast<word>(carry) > static_cast<word>(a));

  a = result;
}

void CPU::sbc(const word address) {
  sbc(bus->read(address));
}

void CPU::scf() {
  setBit(f, CPU_SUB_FLAG, false);
  setBit(f, CPU_HALF_CARRY_FLAG, false);
  setBit(f, CPU_CARRY_FLAG, true);
}

void CPU::ccf() {
  setBit(f, CPU_SUB_FLAG, false);
  setBit(f, CPU_HALF_CARRY_FLAG, false);
  setBit(f, CPU_CARRY_FLAG, !getBit(f, CPU_CARRY_FLAG));
}

void CPU::rlca() {
  f = 0;
  const byte highBit = getBit(a, BIT_7);
  a <<= 1;
  a |= highBit;
  setBit(f, CPU_CARRY_FLAG, highBit);
}

void CPU::callC() {
  callCond(bitIsSet(f, CPU_CARRY_FLAG));
}

void CPU::callNc() {
  callCond(!bitIsSet(f, CPU_CARRY_FLAG));
}

void CPU::callZ() {
  callCond(bitIsSet(f, CPU_ZERO_FLAG));
}

void CPU::callNz() {
  callCond(!bitIsSet(f, CPU_ZERO_FLAG));
}

void CPU::callCond(const bool cond) {
  if (cond) {
    call();
  }
}

void CPU::call() {
  const word returnAddress = fetchWord();
  push(getHighByte(returnAddress), getLowByte(returnAddress));
  pc = returnAddress;
}

void CPU::ld(byte& dst, const byte src) {
  dst = src;
}

void CPU::ld(const word address, const byte src) {
  bus->write(address, src);
}

void CPU::ld(byte& dst, const word address) {
  dst = bus->read(address);
}

void CPU::ld(byte& high, byte& low, const word value) {
  high = getHighByte(value);
  low = getLowByte(value);
}

void CPU::inc(byte& high, byte& low) {
  // Doesn't set flags for some reason
  word value = makeWord(high, low);
  ++value;
  high = getHighByte(value);
  low = getLowByte(value);
}

void CPU::inc(const word address) {
  byte value = bus->read(address);
  inc(value);
  bus->write(address, value);
}

void CPU::inc(byte& dst) {
  const byte value = dst + 1;
  setBit(f, CPU_ZERO_FLAG, value == 0);
  setBit(f, CPU_SUB_FLAG, false);
  setBit(f, CPU_HALF_CARRY_FLAG,
         (dst & MASK_4_LOWER_BYTE) == MASK_4_LOWER_BYTE);
  dst = value;
}

void CPU::dec(byte& high, byte& low) {
  word value = makeWord(high, low);
  --value;
  high = getHighByte(value);
  low = getLowByte(value);
}

void CPU::dec(const word address) {
  byte value = bus->read(address);
  dec(value);
  bus->write(address, value);
}

void CPU::dec(byte& dst) {
  const byte value = dst - 1;
  setBit(f, CPU_ZERO_FLAG, value == 0);
  setBit(f, CPU_SUB_FLAG, true);
  setBit(f, CPU_HALF_CARRY_FLAG, (dst & MASK_4_LOWER_BYTE) == 0);
  dst = value;
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

byte CPU::fetchByte() {
  return bus->read(pc++);
}

word CPU::fetchWord() {
  const byte low = bus->read(pc++);
  const byte high = bus->read(pc++);
  return makeWord(high, low);
}

// Returns the number of T-cycles after execution
qword CPU::execute(const byte opcode) {
  switch (opcode) {
    case 0xCB:
      throw std::invalid_argument("CB opcodes not supported yet!");
    case 0x00:
      nop();
      return 4;
    case 0x01:
      ld(b, c, fetchWord());
      return 12;
    case 0x02:
      ld(makeWord(b, c), a);
      return 8;
    case 0x03:
      inc(b, c);
      return 8;
    case 0x04:
      inc(b);
      return 4;
    default:
      break;
  }

  throw std::invalid_argument("Unknown opcode provided!");
}

}  // namespace gbdmge
