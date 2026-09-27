#include "cpu.h"

CPU::CPU(Memory& memory) : memory {memory}, tCycles {0} {}

void CPU::ora(const byte &src) {
    af.high |= src;
    af.low = 0;
    af.setBit(af.low, ZERO_FLAG, af.high == 0);
}

void CPU::xora(const byte &src) {
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
