#include "CPU.h"

#include <iomanip>

#include "BitOps.h"
#include "Bus.h"
#include "Constants.h"

#include <iostream>
#include <stdexcept>

#include "Disassembler.h"

namespace gbdmge {
CPU::CPU(Bus& bus)
    : ime{false},
      imeEnablePending{false},
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

qword CPU::cycle() {
  qword cycles;
  if (isHalted) {
    // CPU is stopped until an interrupt is pending but still consumes 4 T-cycles
    cycles = 4;
  } else {
    std::cout << "PC: " << "0x" << std::hex << std::setw(4) << std::setfill('0')
              << static_cast<int>(pc) << ": ";
    std::cout << Disassembler::decode(bus->read(pc), bus->read(pc + 1),
                                      bus->read(pc + 2))
              << '\n';  // For debugging purposes, remove later
    const byte opcode = fetchByte();
    cycles = execute(opcode);
  }

  updateIME();
  cycles += handleInterrupts();
  return cycles;
}

void CPU::reset() {
  // Initialize to post boot ROM values
  ime = false;
  imeEnablePending = false;
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

// Returns the number of T-cycles after execution.
// Fuck it, giant switch statement for NOW. There's probably a better approach to this? Maybe?
qword CPU::execute(const byte opcode) {
  switch (opcode) {
    case 0xCB:
      return executeCb(fetchByte());  // Handle CB prefixed instructions.
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
      rlca();
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
      rrca();
      return 4;
    case 0x10:
      // Make sure to consume call fetch byte after instruction, as this takes 2 bytes.
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
      rla();
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
      rra();
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
      ++sp;
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
    case 0x60:
      ld(h, b);
      return 4;
    case 0x61:
      ld(h, c);
      return 4;
    case 0x62:
      ld(h, d);
      return 4;
    case 0x63:
      ld(h, e);
      return 4;
    case 0x64:
      ld(h, h);
      return 4;
    case 0x65:
      ld(h, l);
      return 4;
    case 0x66:
      ld(h, makeWord(h, l));
      return 8;
    case 0x67:
      ld(h, a);
      return 4;
    case 0x68:
      ld(l, b);
      return 4;
    case 0x69:
      ld(l, c);
      return 4;
    case 0x6A:
      ld(l, d);
      return 4;
    case 0x6B:
      ld(l, e);
      return 4;
    case 0x6C:
      ld(l, h);
      return 4;
    case 0x6D:
      ld(l, l);
      return 4;
    case 0x6E:
      ld(l, makeWord(h, l));
      return 8;
    case 0x6F:
      ld(l, a);
      return 4;
    case 0x70:
      ld(makeWord(h, l), b);
      return 8;
    case 0x71:
      ld(makeWord(h, l), c);
      return 8;
    case 0x72:
      ld(makeWord(h, l), d);
      return 8;
    case 0x73:
      ld(makeWord(h, l), e);
      return 8;
    case 0x74:
      ld(makeWord(h, l), h);
      return 8;
    case 0x75:
      ld(makeWord(h, l), l);
      return 8;
    case 0x76:
      halt();
      return 4;
    case 0x77:
      ld(makeWord(h, l), a);
      return 8;
    case 0x78:
      ld(a, b);
      return 4;
    case 0x79:
      ld(a, c);
      return 4;
    case 0x7A:
      ld(a, d);
      return 4;
    case 0x7B:
      ld(a, e);
      return 4;
    case 0x7C:
      ld(a, h);
      return 4;
    case 0x7D:
      ld(a, l);
      return 4;
    case 0x7E:
      ld(a, makeWord(h, l));
      return 8;
    case 0x7F:
      ld(a, a);
      return 4;
    case 0x80:
      add(a, b);
      return 4;
    case 0x81:
      add(a, c);
      return 4;
    case 0x82:
      add(a, d);
      return 4;
    case 0x83:
      add(a, e);
      return 4;
    case 0x84:
      add(a, h);
      return 4;
    case 0x85:
      add(a, l);
      return 4;
    case 0x86:
      add(a, makeWord(h, l));
      return 8;
    case 0x87:
      add(a, a);
      return 4;
    case 0x88:
      add(a, b, true);
      return 4;
    case 0x89:
      add(a, c, true);
      return 4;
    case 0x8A:
      add(a, d, true);
      return 4;
    case 0x8B:
      add(a, e, true);
      return 4;
    case 0x8C:
      add(a, h, true);
      return 4;
    case 0x8D:
      add(a, l, true);
      return 4;
    case 0x8E:
      add(a, makeWord(h, l), true);
      return 8;
    case 0x8F:
      add(a, a, true);
      return 4;
    case 0x90:
      sub(b);
      return 4;
    case 0x91:
      sub(c);
      return 4;
    case 0x92:
      sub(d);
      return 4;
    case 0x93:
      sub(e);
      return 4;
    case 0x94:
      sub(h);
      return 4;
    case 0x95:
      sub(l);
      return 4;
    case 0x96:
      sub(makeWord(h, l));
      return 8;
    case 0x97:
      sub(a);
      return 4;
    case 0x98:
      sub(b, true);
      return 4;
    case 0x99:
      sub(c, true);
      return 4;
    case 0x9A:
      sub(d, true);
      return 4;
    case 0x9B:
      sub(e, true);
      return 4;
    case 0x9C:
      sub(h, true);
      return 4;
    case 0x9D:
      sub(l, true);
      return 4;
    case 0x9E:
      sub(makeWord(h, l), true);
      return 8;
    case 0x9F:
      sub(a, true);
      return 4;
    case 0xA0:
      andA(b);
      return 4;
    case 0xA1:
      andA(c);
      return 4;
    case 0xA2:
      andA(d);
      return 4;
    case 0xA3:
      andA(e);
      return 4;
    case 0xA4:
      andA(h);
      return 4;
    case 0xA5:
      andA(l);
      return 4;
    case 0xA6:
      andA(makeWord(h, l));
      return 8;
    case 0xA7:
      andA(a);
      return 4;
    case 0xA8:
      xorA(b);
      return 4;
    case 0xA9:
      xorA(c);
      return 4;
    case 0xAA:
      xorA(d);
      return 4;
    case 0xAB:
      xorA(e);
      return 4;
    case 0xAC:
      xorA(h);
      return 4;
    case 0xAD:
      xorA(l);
      return 4;
    case 0xAE:
      xorA(makeWord(h, l));
      return 8;
    case 0xAF:
      xorA(a);
      return 4;
    case 0xB0:
      orA(b);
      return 4;
    case 0xB1:
      orA(c);
      return 4;
    case 0xB2:
      orA(d);
      return 4;
    case 0xB3:
      orA(e);
      return 4;
    case 0xB4:
      orA(h);
      return 4;
    case 0xB5:
      orA(l);
      return 4;
    case 0xB6:
      orA(makeWord(h, l));
      return 8;
    case 0xB7:
      orA(a);
      return 4;
    case 0xB8:
      cp(b);
      return 4;
    case 0xB9:
      cp(c);
      return 4;
    case 0xBA:
      cp(d);
      return 4;
    case 0xBB:
      cp(e);
      return 4;
    case 0xBC:
      cp(h);
      return 4;
    case 0xBD:
      cp(l);
      return 4;
    case 0xBE:
      cp(makeWord(h, l));
      return 8;
    case 0xBF:
      cp(a);
      return 4;
    case 0xC0:
      return retNz() ? 20 : 8;
    case 0xC1:
      pop(b, c);
      return 12;
    case 0xC2:
      return jpNz(fetchWord()) ? 16 : 12;
    case 0xC3:
      jp(fetchWord());
      return 16;
    case 0xC4:
      return callNz() ? 24 : 12;
    case 0xC5:
      push(b, c);
      return 16;
    case 0xC6:
      add(a, fetchByte());
      return 8;
    case 0xC7:
      rst(0);
      return 16;
    case 0xC8:
      return retZ() ? 20 : 8;
    case 0xC9:
      ret();
      return 16;
    case 0xCA:
      return jpZ(fetchWord()) ? 16 : 12;
    case 0xCC:
      return callZ() ? 24 : 12;
    case 0xCD:
      call();
      return 24;
    case 0xCE:
      add(a, fetchByte(), true);
      return 8;
    case 0xCF:
      rst(0x08);
      return 16;
    case 0xD0:
      return retNc() ? 20 : 8;
    case 0xD1:
      pop(d, e);
      return 12;
    case 0xD2:
      return jpNc(fetchWord()) ? 16 : 12;
    case 0xD3:
      throw std::invalid_argument("Opcode 0xD3 is invalid!");
    case 0xD4:
      return callNc() ? 24 : 12;
    case 0xD5:
      push(d, e);
      return 16;
    case 0xD6:
      sub(fetchByte());
      return 8;
    case 0xD7:
      rst(0x10);
      return 16;
    case 0xD8:
      return retC() ? 20 : 8;
    case 0xD9:
      retI();
      return 16;
    case 0xDA:
      return jpC(fetchWord()) ? 16 : 12;
    case 0xDB:
      throw std::invalid_argument("Opcode 0xDB is invalid!");
    case 0xDC:
      return callC() ? 24 : 12;
    case 0xDD:
      throw std::invalid_argument("Opcode 0xDD is invalid!");
    case 0xDE:
      sub(fetchByte(), true);
      return 8;
    case 0xDF:
      rst(0x18);
      return 16;
    case 0xE0:
      ld(static_cast<word>(fetchByte() + 0xFF00), a);
      return 12;
    case 0xE1:
      pop(h, l);
      return 12;
    case 0xE2:
      ld(static_cast<word>(c + 0xFF00), a);
      return 8;
    case 0xE3:
    case 0xE4:
      throw std::invalid_argument("Opcodes 0xE3 and 0xE4 are invalid!");
    case 0xE5:
      push(h, l);
      return 16;
    case 0xE6:
      andA(fetchByte());
      return 8;
    case 0xE7:
      rst(0x20);
      return 16;
    case 0xE8:
      addSp(static_cast<sbyte>(fetchByte()));
      return 16;
    case 0xE9:
      jp(makeWord(h, l));
      return 4;
    case 0xEA:
      ld(fetchWord(), a);
      return 16;
    case 0xEB:
    case 0xEC:
    case 0xED:
      throw std::invalid_argument("Opcodes 0xEB, 0xEC, and 0xED are invalid!");
    case 0xEE:
      xorA(fetchByte());
      return 8;
    case 0xEF:
      rst(0x28);
      return 16;
    case 0xF0:
      ld(a, static_cast<word>(fetchByte() + 0xFF00));
      return 12;
    case 0xF1:
      pop(a, f);
      f &= MASK_4_UPPER_BYTE;
      return 12;
    case 0xF2:
      ld(a, static_cast<word>(c + 0xFF00));
      return 8;
    case 0xF3:
      di();
      return 4;
    case 0xF4:
      throw std::invalid_argument("Opcode 0xF4 is invalid!");
    case 0xF5:
      push(a, f);
      return 16;
    case 0xF6:
      orA(fetchByte());
      return 8;
    case 0xF7:
      rst(0x30);
      return 16;
    case 0xF8:
      ldHlSp(static_cast<sbyte>(fetchByte()));
      return 12;
    case 0xF9:
      ldSpHl();
      return 8;
    case 0xFA:
      ld(a, fetchWord());
      return 16;
    case 0xFB:
      ei();
      return 4;
    case 0xFC:
    case 0xFD:
      throw std::invalid_argument("Opcodes 0xFC and 0xFD are invalid!");
    case 0xFE:
      cp(fetchByte());
      return 8;
    case 0xFF:
      rst(0x38);
      return 16;
    default:
      break;
  }

  throw std::invalid_argument("Unknown opcode provided!");
}

// Returns the number of T-cycles after execution.
// Thank god there is an algorithm to parse CB instructions.
qword CPU::executeCb(const byte opcode) {
  const byte operand = opcode & MASK_3_LOWER_BYTE;
  const byte instruction = opcode >> 6;
  const byte immediate = opcode >> 3 & MASK_3_LOWER_BYTE;

  byte* reg{nullptr};
  bool isMemory{false};
  switch (operand) {
    case 0:
      reg = &b;
      break;
    case 1:
      reg = &c;
      break;
    case 2:
      reg = &d;
      break;
    case 3:
      reg = &e;
      break;
    case 4:
      reg = &h;
      break;
    case 5:
      reg = &l;
      break;
    case 6:
      isMemory = true;
      break;
    case 7:
      reg = &a;
      break;
    default:
      throw std::invalid_argument("Unknown target in CB opcode!");
  }

  const word address = makeWord(h, l);
  byte value = isMemory ? bus->read(address) : *reg;
  switch (instruction) {
    case 0:
      switch (immediate) {
        case 0:
          rlc(value);
          break;
        case 1:
          rrc(value);
          break;
        case 2:
          rl(value);
          break;
        case 3:
          rr(value);
          break;
        case 4:
          sla(value);
          break;
        case 5:
          sra(value);
          break;
        case 6:
          swap(value);
          break;
        case 7:
          srl(value);
          break;
        default:
          throw std::runtime_error("Unknown immediate in CB opcode!");
      }
      break;
    case 1:
      bit(value, immediate);
      break;
    case 2:
      set(value, immediate, false);  // RES
      break;
    case 3:
      set(value, immediate, true);  // SET
      break;
    default:
      throw std::invalid_argument("Unknown instruction in CB opcode!");
  }

  if (instruction != 1) {
    if (isMemory) {
      bus->write(address, value);
    } else {
      *reg = value;
    }
  }

  if (isMemory) {
    return instruction == 1 ? 12 : 16;
  }

  return 8;
}

void CPU::updateIME() {
  if (imeEnablePending) {
    ime = true;
    imeEnablePending = false;
  }
}

qword CPU::handleInterrupts() {
  const bool pending = interruptsPending();

  if (pending && isHalted) {
    isHalted = false;
  }

  if (!ime || !pending) {
    return 0;
  }

  const byte ie = bus->read(MEMORY_IO_IE);
  byte iff = bus->read(MEMORY_IO_IF);

  for (byte i = 0; i < 5; ++i) {
    if (bitIsSet(ie, i) && bitIsSet(iff, i)) {
      push(getHighByte(pc), getLowByte(pc));

      setBit(iff, i, false);
      bus->write(MEMORY_IO_IF, iff);

      ime = false;
      imeEnablePending = false;

      switch (i) {
        case INTERRUPT_VBLANK_FLAG:
          pc = MEMORY_INTERRUPT_VBLANK;
          break;
        case INTERRUPT_LCD_FLAG:
          pc = MEMORY_INTERRUPT_LCD;
          break;
        case INTERRUPT_TIMER_FLAG:
          pc = MEMORY_INTERRUPT_TIMER;
          break;
        case INTERRUPT_SERIAL_FLAG:
          pc = MEMORY_INTERRUPT_SERIAL;
          break;
        case INTERRUPT_JOYPAD_FLAG:
          pc = MEMORY_INTERRUPT_JOYPAD;
          break;
        default:
          throw std::invalid_argument("Unknown interrupt flag!");
      }
      break;
    }
  }
  return 20;
}

bool CPU::interruptsPending() const {
  return (bus->read(MEMORY_IO_IE) & bus->read(MEMORY_IO_IF) &
          MASK_5_LOWER_BYTE) != 0;
}

void CPU::pop(byte& high, byte& low) {
  high = bus->read(sp + 1);
  low = bus->read(sp);
  sp += 2;
}

void CPU::push(const byte high, const byte low) {
  bus->write(sp - 1, high);
  bus->write(sp - 2, low);
  sp -= 2;
}

void CPU::andA(const word address) {
  andA(bus->read(address));
}

void CPU::andA(const byte src) {
  a = a & src;
  f = 0;
  setBit(f, CPU_ZERO_FLAG, a == 0);
  setBit(f, CPU_HALF_CARRY_FLAG, true);
}

void CPU::orA(const word address) {
  orA(bus->read(address));
}

void CPU::orA(const byte src) {
  a = a | src;
  f = 0;
  setBit(f, CPU_ZERO_FLAG, a == 0);
}

void CPU::xorA(const word address) {
  xorA(bus->read(address));
}

void CPU::xorA(const byte src) {
  a = a ^ src;
  f = 0;
  setBit(f, CPU_ZERO_FLAG, a == 0);
}

void CPU::cpl() {
  a = ~a;
  setBit(f, CPU_SUB_FLAG, true);
  setBit(f, CPU_HALF_CARRY_FLAG, true);
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

void CPU::jp(const word address) {
  jpCond(address, true);
}

bool CPU::jpCond(const word address, const bool cond) {
  if (cond) {
    pc = address;
    return true;
  }
  return false;
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

void CPU::jr(const sbyte src) {
  jrCond(src, true);
}

bool CPU::jrCond(const sbyte src, const bool cond) {
  if (cond) {
    pc = static_cast<word>(static_cast<sdword>(pc) + static_cast<sdword>(src));
    return true;
  }
  return false;
}

void CPU::rst(const word address) {
  push(getHighByte(pc), getLowByte(pc));
  pc = address;
}

void CPU::nop() {}

void CPU::ei() {
  // IME is only set after the instruction following EI
  imeEnablePending = true;
}

void CPU::di() {
  ime = false;
  imeEnablePending = false;  // Disable pending EI
}

bool CPU::retI() {
  throw std::runtime_error("RETI not implemented!");
}

bool CPU::retC() {
  return retCond(bitIsSet(f, CPU_CARRY_FLAG));
}

bool CPU::retZ() {
  return retCond(bitIsSet(f, CPU_ZERO_FLAG));
}

bool CPU::retNc() {
  return retCond(!bitIsSet(f, CPU_CARRY_FLAG));
}

bool CPU::retNz() {
  return retCond(!bitIsSet(f, CPU_ZERO_FLAG));
}

bool CPU::retCond(const bool cond) {
  if (cond) {
    ret();
    return true;
  }
  return false;
}

void CPU::ret() {
  pc = makeWord(bus->read(sp + 1), bus->read(sp));
  sp += 2;
}

void CPU::halt() {
  // This may need to change down the line
  if (ime) {
    isHalted = true;
  }
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

bool CPU::callC() {
  return callCond(bitIsSet(f, CPU_CARRY_FLAG));
}

bool CPU::callNc() {
  return callCond(!bitIsSet(f, CPU_CARRY_FLAG));
}

bool CPU::callZ() {
  return callCond(bitIsSet(f, CPU_ZERO_FLAG));
}

bool CPU::callNz() {
  return callCond(!bitIsSet(f, CPU_ZERO_FLAG));
}

bool CPU::callCond(const bool cond) {
  if (cond) {
    call();
    return true;
  }

  fetchWord();
  return false;
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

void CPU::ldSpHl() {
  sp = makeWord(h, l);
}

void CPU::ldHlSp(const sbyte src) {
  const word result = sp + src;
  f = 0;
  h = getHighByte(result);
  l = getLowByte(result);
  setBit(
      f, CPU_HALF_CARRY_FLAG,
      (sp & MASK_4_LOWER_WORD) + (src & MASK_4_LOWER_WORD) > MASK_4_LOWER_WORD);
  setBit(
      f, CPU_CARRY_FLAG,
      (sp & MASK_8_LOWER_WORD) + (src & MASK_8_LOWER_WORD) > MASK_8_LOWER_WORD);
}

void CPU::daa() {
  byte adjustment = 0;

  const bool subtractFlag = bitIsSet(f, CPU_SUB_FLAG);
  const bool halfCarryFlag = bitIsSet(f, CPU_HALF_CARRY_FLAG);
  const bool carryFlag = bitIsSet(f, CPU_CARRY_FLAG);

  if (halfCarryFlag ||
      (!subtractFlag && (a & MASK_4_LOWER_BYTE) > BCD_PLACE_MAXIMUM_BYTE)) {
    adjustment |= BCD_ONES_PLACE_ADJUST_BYTE;
  }

  if (carryFlag || (!subtractFlag && a > BCD_MAXIMUM_BYTE)) {
    adjustment |= BCD_TENS_PLACE_ADJUST_BYTE;
    setBit(f, CPU_CARRY_FLAG, true);
  }

  if (subtractFlag) {
    a -= adjustment;
  } else {
    a += adjustment;
  }

  setBit(f, CPU_ZERO_FLAG, a == 0);
  setBit(f, CPU_HALF_CARRY_FLAG, false);
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

void CPU::add(byte& dst, const word address, const bool useCarry) {
  add(dst, bus->read(address), useCarry);
}

void CPU::add(byte& dst, const byte src, const bool useCarry) {
  const word carry = useCarry && bitIsSet(f, CPU_CARRY_FLAG);
  const word sum = static_cast<word>(dst) + static_cast<word>(src) + carry;
  const byte result = static_cast<byte>(sum);

  setBit(f, CPU_ZERO_FLAG, result == 0);
  setBit(f, CPU_SUB_FLAG, false);
  setBit(f, CPU_HALF_CARRY_FLAG,
         (dst & MASK_4_LOWER_BYTE) + (src & MASK_4_LOWER_BYTE) + carry >
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

void CPU::sub(const word address, const bool useCarry) {
  sub(bus->read(address), useCarry);
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

void CPU::rlca() {
  shift(a, false, true);
  setBit(f, CPU_ZERO_FLAG, false);
}

void CPU::rrca() {
  shift(a, false, false);
  setBit(f, CPU_ZERO_FLAG, false);
}

void CPU::rla() {
  shift(a, true, true);
  setBit(f, CPU_ZERO_FLAG, false);
}

void CPU::rra() {
  shift(a, true, false);
  setBit(f, CPU_ZERO_FLAG, false);
}

void CPU::rlc(byte& dst) {
  shift(dst, false, true);
}

void CPU::rrc(byte& dst) {
  shift(dst, false, false);
}

void CPU::rl(byte& dst) {
  shift(dst, true, true);
}

void CPU::rr(byte& dst) {
  shift(dst, true, false);
}

void CPU::sla(byte& dst) {
  shiftCb(dst, false, true);
}

void CPU::sra(byte& dst) {
  shiftCb(dst, true, false);
}

void CPU::srl(byte& dst) {
  shiftCb(dst, false, false);
}

void CPU::shift(byte& dst, const bool throughCarry, const bool shiftLeft) {
  const bool selectedBit = bitIsSet(dst, shiftLeft ? BIT_7_BYTE : BIT_0_BYTE);

  if (shiftLeft) {
    dst <<= 1;
  } else {
    dst >>= 1;
  }

  const byte insertPos = shiftLeft ? BIT_0_BYTE : BIT_7_BYTE;
  if (throughCarry) {
    setBit(dst, insertPos, bitIsSet(f, CPU_CARRY_FLAG));
  } else {
    setBit(dst, insertPos, selectedBit);
  }

  f = 0;
  setBit(f, CPU_ZERO_FLAG, dst == 0);
  setBit(f, CPU_CARRY_FLAG, selectedBit);
}

void CPU::shiftCb(byte& dst, const bool preserveSign, const bool shiftLeft) {
  const bool selectedBit = bitIsSet(dst, shiftLeft ? BIT_7_BYTE : BIT_0_BYTE);
  const bool highBit = bitIsSet(dst, BIT_7_BYTE);

  if (shiftLeft) {
    dst <<= 1;
  } else {
    dst >>= 1;
  }

  if (preserveSign) {
    setBit(dst, BIT_7_BYTE, highBit);
  }

  f = 0;
  setBit(f, CPU_ZERO_FLAG, dst == 0);
  setBit(f, CPU_CARRY_FLAG, selectedBit);
}

void CPU::set(byte& dst, const byte pos, const bool set) {
  setBit(dst, pos, set);
}

void CPU::swap(byte& dst) {
  f = 0;
  dst = dst >> 4 | dst << 4;
  setBit(f, CPU_ZERO_FLAG, dst == 0);
}

void CPU::bit(const word address, const byte pos) {
  bit(bus->read(address), pos);
}

void CPU::bit(const byte src, const byte pos) {
  setBit(f, CPU_ZERO_FLAG, !getBit(src, pos));
  setBit(f, CPU_SUB_FLAG, false);
  setBit(f, CPU_HALF_CARRY_FLAG, true);
}
}  // namespace gbdmge
