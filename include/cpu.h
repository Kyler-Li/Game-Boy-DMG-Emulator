#ifndef GBDMGE_CPU_H
#define GBDMGE_CPU_H

#include "types.h"
#include "memory.h"

namespace gbdmge {
    class CPU{
        Memory& memory;

        // For each Reg, the rightmost letter represents low byte,
        // leftmost letter represents high byte. For example, the f register
        // would be accessed as using getLow method.
        Reg af;
        Reg bc;
        Reg de;
        Reg hl;
        Reg sp;
        Reg pc;

        qword tCycles; // 1 T-cycle = 1 clock cycle, 1 M-cycle = 4 clock cycles = 4 T-cycles

        const byte ZERO_FLAG = 7, SUB_FLAG = 6, HALF_CARRY_FLAG = 5, CARRY_FLAG = 4;

        void pop(Reg& src);
        void push(Reg& src);

        void anda(byte src);
        void ora(byte src);
        void xora(byte src);

        void cp(byte b);
        void cp(Reg& b);

        void jpz(word address);
        void jpnz(word address);
        void jpc(word address);
        void jpnc(word address);
        void jp(Reg& dst);
        void jp(word address);
        void jpCond(word address, bool cond);
        void nop();
        void cpl();
    public:
        explicit CPU(Memory& memory);
        byte fetch();
        qword execute(byte opcode);
        void reset();
    };
}

#endif
