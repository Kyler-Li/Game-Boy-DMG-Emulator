#include <stdexcept>
#include "ppu.h"
#include "memory.h"

namespace gbdmge {
    PPU::PPU(Memory& vram) : vram {vram} {}
}
