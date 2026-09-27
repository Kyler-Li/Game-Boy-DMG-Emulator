#pragma once

#include "types.h"
#include "memory.h"

class CPU{
    Memory& memory;
    Reg af;
    Reg bc;
    Reg de;
    Reg hl;
    Reg sp;
    Reg pc;

    qword tCycles; // 1 T-cycle = 1 clock cycle, 1 M-cycle = 4 clock cycles = 4 T-cycles

    const byte ZERO_FLAG = 7, SUB_FLAG = 6, HALF_CARRY_FLAG = 5, CARRY_FLAG = 4;

    void ora(const byte& src);
    void xora(const byte& src);
public:
    explicit CPU(Memory& memory);
    byte fetch();
    qword execute(byte opcode);
    void reset();
};