#include "cpu.h"
#include "console.h"
#include "constants.h"

#include <stdexcept>

namespace gbdmge {
CPU::CPU(Console& console)
    : ime{false},  // Interrupts are disabled on boot
      console{&console},
      tCycles{0} {}

void CPU::pop(Register& src) {
  const word address = sp.get();
  src.setLow(console->memory.read(address));
  src.setHigh(console->memory.read(address + 1));
  sp.inc();
  sp.inc();
}

void CPU::push(Register& src) {
  const word address = sp.get();
  console->memory.write(address - 1, src.getHigh());
  console->memory.write(address - 2, src.getLow());
  sp.dec();
  sp.dec();
}

void CPU::anda(const byte src) {
  af.setHigh(af.getHigh() & src);
  af.setLow(0);

  byte& low = af.getLow();
  af.setBit(low, CPU_ZERO_FLAG, af.getHigh() == 0);
  af.setBit(low, CPU_HALF_CARRY_FLAG, true);
}

void CPU::ora(const byte src) {
  af.setHigh(af.getHigh() | src);
  af.setLow(0);
  af.setBit(af.getLow(), CPU_ZERO_FLAG, af.getHigh() == 0);
}

void CPU::xora(const byte src) {
  af.setHigh(af.getHigh() ^ src);
  af.setLow(0);
  af.setBit(af.getLow(), CPU_ZERO_FLAG, af.getHigh() == 0);
}

void CPU::cp(const byte b) {
  const byte a = af.getHigh();

  byte& low = af.getLow();
  af.setBit(low, CPU_ZERO_FLAG, a == b);
  af.setBit(low, CPU_SUB_FLAG, true);
  af.setBit(low, CPU_HALF_CARRY_FLAG,
            (a & MASK_4_LOWER_BYTE) < (b & MASK_4_LOWER_BYTE));
  af.setBit(low, CPU_CARRY_FLAG, a < b);
}

void CPU::cp(Register& b) {
  cp(console->memory.read(b.get()));
}

void CPU::jpz(word address) {
  jpCond(address, af.bitIsSet(af.getLow(), CPU_ZERO_FLAG));
}

void CPU::jpnz(const word address) {
  jpCond(address, !af.bitIsSet(af.getLow(), CPU_ZERO_FLAG));
}

void CPU::jpc(const word address) {
  jpCond(address, af.bitIsSet(af.getLow(), CPU_CARRY_FLAG));
}

void CPU::jpnc(const word address) {
  jpCond(address, !af.bitIsSet(af.getLow(), CPU_CARRY_FLAG));
}

void CPU::jp(Register& dst) {
  jpCond(dst.get(), true);
}

void CPU::jp(const word address) {
  jpCond(address, true);
}

void CPU::jpCond(const word address, const bool cond) {
  if (cond) {
    pc.set(address);
  }
}

void CPU::nop() {}

void CPU::cpl() {
  af.setHigh(~af.getHigh());
  byte& low = af.getLow();
  af.setBit(low, CPU_HALF_CARRY_FLAG, true);
  af.setBit(low, CPU_SUB_FLAG, true);
}

void CPU::ei() {
  ime = true;
}

void CPU::di() {
  ime = false;
}

void CPU::jrC(const byte src) {
  jrCond(src, af.bitIsSet(af.getLow(), CPU_CARRY_FLAG));
}

void CPU::jrNc(const byte src) {
  jrCond(src, !af.bitIsSet(af.getLow(), CPU_CARRY_FLAG));
}

void CPU::jrZ(const byte src) {
  jrCond(src, af.bitIsSet(af.getLow(), CPU_ZERO_FLAG));
}

void CPU::jrNz(const byte src) {
  jrCond(src, !af.bitIsSet(af.getLow(), CPU_ZERO_FLAG));
}

void CPU::jr(const byte src) {
  jrCond(src, true);
}

void CPU::jrCond(const byte src, const bool cond) {
  if (cond) {
    const sbyte off = static_cast<sbyte>(src);
    pc.set(pc.get() + off);
  }
}

void CPU::reset() {
  // Initialize to post boot ROM values
  ime = false;
  af.set(0x01B0);
  bc.set(0x0013);
  de.set(0x00D8);
  hl.set(0x014D);
  sp.set(0xFFFE);
  pc.set(0x0100);
}

byte CPU::fetch() {
  const word address = pc.get();
  pc.inc();
  return console->memory.read(address);
}

// Returns the number of T-cycles after execution
qword CPU::execute(byte opcode) {
  throw std::logic_error("not implemented");
}
}  // namespace gbdmge
