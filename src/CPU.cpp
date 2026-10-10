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
  bus->write(sp - 1, high);
  bus->write(sp - 2, low);
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

bool CPU::jpZ(const word address) {
  return jpCond(address, bitIsSet(f, CPU_ZERO_FLAG));
}

bool CPU::jpNz(const word address) {
  return jpCond(address, !bitIsSet(f, CPU_ZERO_FLAG));
}

bool CPU::jpC(const word address) {
  return jpCond(address, bitIsSet(f, CPU_CARRY_FLAG));
}

bool CPU::jpNc(const word address) {
  return jpCond(address, !bitIsSet(f, CPU_CARRY_FLAG));
}

bool CPU::jp(const word address) {
  return jpCond(address, true);
}

bool CPU::jpCond(const word address, const bool cond) {
  if (cond) {
    pc = address;
    return true;
  }
  return false;
}

void CPU::nop() {}

void CPU::cpl() {
  a = ~a;
  setBit(f, CPU_SUB_FLAG, true);
  setBit(f, CPU_HALF_CARRY_FLAG, true);
}

void CPU::ei() {
  throw std::runtime_error("DI not implemented!");
}

void CPU::di() {
  throw std::runtime_error("DI not implemented!");
}

bool CPU::jrC(const sbyte src) {
  return jrCond(src, bitIsSet(f, CPU_CARRY_FLAG));
}

bool CPU::jrNc(const sbyte src) {
  return jrCond(src, !bitIsSet(f, CPU_CARRY_FLAG));
}

bool CPU::jrZ(const sbyte src) {
  return jrCond(src, bitIsSet(f, CPU_ZERO_FLAG));
}

bool CPU::jrNz(const sbyte src) {
  return jrCond(src, !bitIsSet(f, CPU_ZERO_FLAG));
}

bool CPU::jr(const sbyte src) {
  return jrCond(src, true);
}

bool CPU::jrCond(const sbyte src, const bool cond) {
  if (cond) {
    pc = static_cast<word>(static_cast<sdword>(pc) + static_cast<sdword>(src));
    return true;
  }
  return false;
}

void CPU::retI() {
  throw std::runtime_error("DI not implemented!");
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
  throw std::runtime_error("HALT not implemented!");
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
  } else {
    fetchWord();  // Consume target address
  }
}

void CPU::call() {
  const word returnAddress = pc + 2;
  push(getHighByte(returnAddress), getLowByte(returnAddress));
  pc = fetchWord();
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

void CPU::ldSp(const word address) {
  bus->write(address, getLowByte(sp));
  bus->write(address + 1, getHighByte(sp));
}

void CPU::ldSp() {
  sp = fetchWord();
}

void CPU::daa() {
  throw std::runtime_error("DAA not implemented!");
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

void CPU::decSp() {
  --sp;
}

void CPU::add(byte& dst, const word address) {
  add(dst, bus->read(address));
}

void CPU::add(byte& dst, const byte src) {
  const word sum = static_cast<word>(dst) + static_cast<word>(src);
  const byte result = static_cast<byte>(sum);

  setBit(f, CPU_ZERO_FLAG, result == 0);
  setBit(f, CPU_SUB_FLAG, false);
  setBit(f, CPU_HALF_CARRY_FLAG,
         (dst & MASK_4_LOWER_BYTE) + (src & MASK_4_LOWER_BYTE) >
             MASK_4_LOWER_BYTE);
  setBit(f, CPU_CARRY_FLAG, sum > MASK_8_LOWER_BYTE);
  dst = result;
}

void CPU::addHl(const byte high, const byte low) {
  const word value = makeWord(high, low);
  const word hl = makeWord(h, l);
  const dword sum = static_cast<dword>(hl) + static_cast<dword>(value);

  // Does not set zero flag for some reason
  setBit(f, CPU_SUB_FLAG, false);
  setBit(f, CPU_HALF_CARRY_FLAG,
         (hl & MASK_12_LOWER_WORD) + (value & MASK_12_LOWER_WORD) >
             MASK_12_LOWER_WORD);
  setBit(f, CPU_CARRY_FLAG, sum > MASK_16_LOWER_WORD);

  const word result = static_cast<word>(sum);
  h = getHighByte(result);
  l = getLowByte(result);
}

void CPU::addSp(const sbyte src) {
  const byte offset = static_cast<byte>(src);

  const word result =
      static_cast<word>(static_cast<sdword>(sp) + static_cast<sdword>(src));

  setBit(f, CPU_ZERO_FLAG, false);
  setBit(f, CPU_SUB_FLAG, false);
  setBit(f, CPU_HALF_CARRY_FLAG,
         (sp & MASK_4_LOWER_BYTE) + (offset & MASK_4_LOWER_BYTE) >
             MASK_4_LOWER_BYTE);
  setBit(f, CPU_CARRY_FLAG,
         (sp & MASK_8_LOWER_BYTE) + offset > MASK_8_LOWER_BYTE);

  sp = result;
}

void CPU::sbc(const byte src) {
  sub(src, true);
}

void CPU::sbc(const word address) {
  sbc(bus->read(address));
}

void CPU::sub(const word address) {
  sub(bus->read(address), false);
}

void CPU::sub(const byte src, const bool useCarry) {
  const word carry = useCarry && bitIsSet(f, CPU_CARRY_FLAG);
  const word value = static_cast<word>(src) + static_cast<word>(carry);
  const byte result = static_cast<byte>(a - value);

  setBit(f, CPU_ZERO_FLAG, result == 0);
  setBit(f, CPU_SUB_FLAG, true);
  setBit(f, CPU_HALF_CARRY_FLAG,
         (a & MASK_4_LOWER_BYTE) < (src & MASK_4_LOWER_BYTE) + carry);
  setBit(f, CPU_CARRY_FLAG, static_cast<word>(a) < value);

  a = result;
}

void CPU::set(byte& dst, const byte pos, const bool set) {
  setBit(dst, pos, set);
}

void CPU::set(const word address, const byte pos, const bool set) {
  byte value = bus->read(address);
  setBit(value, pos, set);
  bus->write(address, value);
}

void CPU::swap(byte& dst) {
  f = 0;
  dst = dst >> 4 | dst << 4;
  setBit(f, CPU_ZERO_FLAG, dst == 0);
}

void CPU::swap(const word address) {
  byte value = bus->read(address);
  swap(value);
  bus->write(address, value);
}

void CPU::bit(const word address, const byte pos) {
  bit(bus->read(address), pos);
}

void CPU::bit(const byte src, const byte pos) {
  setBit(f, CPU_ZERO_FLAG, !getBit(src, pos));
  setBit(f, CPU_SUB_FLAG, false);
  setBit(f, CPU_HALF_CARRY_FLAG, true);
}

void CPU::rst(const word address) {
  push(getHighByte(pc), getLowByte(pc));
  pc = address;
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
    case 0x05:
      dec(b);
      return 4;
    case 0x06:
      ld(b, fetchByte());
      return 8;
    case 0x07:
      throw std::invalid_argument("RCLA not implemented!");
      return 4;
    case 0x08:
      ldSp(fetchWord());
      return 20;
    case 0x09:
      addHl(b, c);
      return 8;
    case 0x0A:
      ld(a, makeWord(b, c));
      return 8;
    case 0x0B:
      dec(b, c);
      return 8;
    case 0x0C:
      inc(c);
      return 4;
    case 0x0D:
      dec(c);
      return 4;
    case 0x0E:
      ld(c, fetchByte());
      return 8;
    case 0x0F:
      throw std::invalid_argument("RCCA not implemented!");
      return 4;
    case 0x10:
      throw std::invalid_argument("STOP not implemented!");
      return 4;
    case 0x11:
      ld(d, e, fetchWord());
      return 12;
    case 0x12:
      ld(makeWord(d, e), a);
      return 8;
    case 0x13:
      inc(d, e);
      return 8;
    case 0x14:
      inc(d);
      return 4;
    case 0x15:
      dec(d);
      return 4;
    case 0x16:
      ld(d, fetchByte());
      return 8;
    case 0x17:
      throw std::invalid_argument("RLA not implemented!");
      return 4;
    case 0x18:
      jr(static_cast<sbyte>(fetchByte()));
      return 12;
    case 0x19:
      addHl(d, e);
      return 8;
    case 0x1A:
      ld(a, makeWord(d, e));
      return 8;
    case 0x1B:
      dec(d, e);
      return 8;
    case 0x1C:
      inc(e);
      return 4;
    case 0x1D:
      dec(e);
      return 4;
    case 0x1E:
      ld(e, fetchByte());
      return 8;
    case 0x1F:
      throw std::invalid_argument("RRA not implemented!");
      return 4;
    case 0x20:
      return jrNz(static_cast<sbyte>(fetchByte())) ? 12 : 8;
    case 0x21:
      ld(h, l, fetchWord());
      return 12;
    case 0x22:
      ld(makeWord(h, l), a);
      addToPair(h, l, 1);
      return 8;
    case 0x23:
      inc(h, l);
      return 8;
    case 0x24:
      inc(h);
      return 4;
    case 0x25:
      dec(h);
      return 4;
    case 0x26:
      ld(h, fetchByte());
      return 8;
    case 0x27:
      daa();
      return 4;
    case 0x28:
      return jrZ(static_cast<sbyte>(fetchByte())) ? 12 : 8;
    case 0x29:
      addHl(h, l);
      return 8;
    case 0x2A:
      ld(a, makeWord(h, l));
      addToPair(h, l, 1);
      return 8;
    case 0x2B:
      dec(h, l);
      return 8;
    case 0x2C:
      inc(l);
      return 4;
    case 0x2D:
      dec(l);
      return 4;
    case 0x2E:
      ld(l, fetchByte());
      return 8;
    case 0x2F:
      cpl();
      return 4;
    case 0x30:
      return jrNc(static_cast<sbyte>(fetchByte())) ? 12 : 8;
    case 0x31:
      ldSp();
      return 12;
    case 0x32:
      ld(makeWord(h, l), a);
      addToPair(h, l, -1);
      return 8;
    case 0x33:
      inc(sp);
      return 8;
    case 0x34:
      inc(makeWord(h, l));
      return 12;
    case 0x35:
      dec(makeWord(h, l));
      return 12;
    case 0x36:
      ld(makeWord(h, l), fetchByte());
      return 12;
    case 0x37:
      scf();
      return 4;
    case 0x38:
      return jrC(static_cast<sbyte>(fetchByte())) ? 12 : 8;
    case 0x39:
      addHl(getHighByte(sp), getLowByte(sp));
      return 8;
    case 0x3A:
      ld(a, makeWord(h, l));
      addToPair(h, l, -1);
      return 8;
    case 0x3B:
      decSp();
      return 8;
    case 0x3C:
      inc(a);
      return 4;
    case 0x3D:
      dec(a);
      return 4;
    case 0x3E:
      ld(a, fetchByte());
      return 8;
    case 0x3F:
      ccf();
      return 4;
    case 0x40:
      ld(b, b);
      return 4;
    case 0x41:
      ld(b, c);
      return 4;
    case 0x42:
      ld(b, d);
      return 4;
    case 0x43:
      ld(b, e);
      return 4;
    case 0x44:
      ld(b, h);
      return 4;
    case 0x45:
      ld(b, l);
      return 4;
    case 0x46:
      ld(b, makeWord(h, l));
      return 8;
    case 0x47:
      ld(b, a);
      return 4;
    case 0x48:
      ld(c, b);
      return 4;
    case 0x49:
      ld(c, c);
      return 4;
    case 0x4A:
      ld(c, d);
      return 4;
    case 0x4B:
      ld(c, e);
      return 4;
    case 0x4C:
      ld(c, h);
      return 4;
    case 0x4D:
      ld(c, l);
      return 4;
    case 0x4E:
      ld(c, makeWord(h, l));
      return 8;
    case 0x4F:
      ld(c, a);
      return 4;
    case 0x50:
      ld(d, b);
      return 4;
    case 0x51:
      ld(d, c);
      return 4;
    case 0x52:
      ld(d, d);
      return 4;
    case 0x53:
      ld(d, e);
      return 4;
    case 0x54:
      ld(d, h);
      return 4;
    case 0x55:
      ld(d, l);
      return 4;
    case 0x56:
      ld(d, makeWord(h, l));
      return 8;
    case 0x57:
      ld(d, a);
      return 4;
    case 0x58:
      ld(e, b);
      return 4;
    case 0x59:
      ld(e, c);
      return 4;
    case 0x5A:
      ld(e, d);
      return 4;
    case 0x5B:
      ld(e, e);
      return 4;
    case 0x5C:
      ld(e, h);
      return 4;
    case 0x5D:
      ld(e, l);
      return 4;
    case 0x5E:
      ld(e, makeWord(h, l));
      return 8;
    case 0x5F:
      ld(e, a);
      return 4;
    default:
      break;
  }

  throw std::invalid_argument("Unknown opcode provided!");
}

}  // namespace gbdmge
