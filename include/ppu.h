#ifndef GBDMGE_PPU_H
#define GBDMGE_PPU_H

#include "types.h"
#include "memory.h"

namespace gbdmge {
    class PPU{
        Memory& vram;
    public:
        explicit PPU(Memory& vram);
    };
}

#endif
