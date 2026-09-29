#ifndef GBDMGE_MEMORY_H
#define GBDMGE_MEMORY_H

#include <array>
#include "types.h"

namespace gbdmge {
    class MemoryBC {

    };

    class Memory{
        // Need to add:
        // 0000 - 7FFF, ROM banks
        // A000 - BFFF, external RAM
        // These regions are managed by a Memory Bank Controller
        // depending on the size of the game ROM to be loaded

        const word VW_RANGE_MASK = 0x1FFF;
        std::array<byte, 0x2000> vram; // Video ram, 8000 - 9FFF
        std::array<byte, 0x2000> wram; // Work ram, C000 - DFFF

        const word OZ_RANGE_MASK = 0x00FF;
        std::array<byte, 0x0100> oram; // FE00 - FE9F
        std::array<byte, 0x0100> ioram; // FF00 - FFFF

        // IO Registers, which are mapped to address in range FF00 - FFFF
        MemoryReg p1; // Joy pad
        MemoryReg sb, sc; // Serial transfer
        MemoryReg div, tima, tma, tac; // Timer and divider
        MemoryReg ie, inf; // Interrupts

        MemoryReg nr10, nr11, nr12, nr13, nr14; // Sound channel 1 - sweep
        MemoryReg nr21, nr22, nr23, nr24; // Sound channel 2 - pulse
        MemoryReg nr30, nr31, nr32, nr33, nr34; // Sound channel 3 - wave
        MemoryReg nr41, nr42, nr43, nr44; // Sound channel 4 - noise
        MemoryReg nr50, nr51, nr52; // Sound global control

        const word WAVE_RANGE_MASK = 0x000F;
        std::array<byte, 0x10> wavePattern; // Wave pattern

        MemoryReg lcdc, ly, lyc, scy, scx, bgp, obp0, obp1; // LCD control
        MemoryReg stat, wy, wx; // LCD control
        MemoryReg dma; // Direct memory access

        friend class CPU;
    public:
        Memory();
        void write(word address, byte value);
        byte read(word address) const;
        void reset();
    };
}
#endif
