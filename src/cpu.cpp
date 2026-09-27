#include "cpu.h"

CPU::CPU(Memory& memory) : memory {memory}, tCycles {0} {}

void CPU::pop(Reg& src) {
    const word address = static_cast<word>(sp);
    src.low = memory.read(address);
    src.high = memory.read(address + 1);
    ++sp;
    ++sp;
}

void CPU::push(const Reg& src) {
    const word address = static_cast<word>(sp);
    memory.write(address - 1, src.high);
    memory.write(address - 2, src.low);
    --sp;
    --sp;
}

void CPU::anda(const byte& src) {
    af.high &= src;
    af.low = 0;
    af.setBit(af.low, ZERO_FLAG, af.high == 0);
    af.setBit(af.low, HALF_CARRY_FLAG, true);
}

void CPU::ora(const byte& src) {
    af.high |= src;
    af.low = 0;
    af.setBit(af.low, ZERO_FLAG, af.high == 0);
}

void CPU::xora(const byte& src) {
    af.high ^= src;
    af.low = 0;
    af.setBit(af.low, ZERO_FLAG, af.high == 0);
}

void CPU::reset() {
    // Initialize registers to post boot ROM values
    af = 0x01B0;
    bc = 0x0013;
    de = 0x00D8;
    hl = 0x014D;
    sp = 0xFFFE;
    pc = 0x0100;
}

byte CPU::fetch() {
    const word address = static_cast<word>(pc);
    ++pc;
    return memory.read(address);
}

// Returns the number of T-cycles after execution
qword CPU::execute(byte opcode) {
    return 0;
}
