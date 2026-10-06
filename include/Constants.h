#ifndef GBDMGE_ENUMLATOR_H
#define GBDMGE_ENUMLATOR_H

#include <array>
#include "Types.h"

namespace gbdmge {
// Masks upper bits
static constexpr word MASK_4_UPPER_WORD = 0xF000;

// Masks lower bits
static constexpr byte MASK_1_LOWER_BYTE = 0x01;
static constexpr byte MASK_2_LOWER_BYTE = 0x03;
static constexpr byte MASK_4_LOWER_BYTE = 0x0F;
static constexpr byte MASK_5_LOWER_BYTE = 0x1F;
static constexpr byte MASK_8_LOWER_BYTE = 0xFF;
static constexpr word MASK_13_LOWER_WORD = 0x1FFF;

// Mask for specific bits
static constexpr byte MASK_5_6_BYTE = 0x60;

// Indexes for specific bits
static constexpr byte BIT_7 = 7;

// Positions for flags register in CPU
// Flags register layout:
// Bit:     |   7  |      6      |     5      |   4   |  3 - 0  |
// Meaning: | Zero | Subtraction | Half Carry | Carry |  Unused |
static constexpr byte CPU_ZERO_FLAG = 7;
static constexpr byte CPU_SUB_FLAG = 6;
static constexpr byte CPU_HALF_CARRY_FLAG = 5;
static constexpr byte CPU_CARRY_FLAG = 4;

// Nintendo Logo
static constexpr word NINTENDO_LOGO_START = 0x0104;
static constexpr word NINTENDO_LOGO_END = 0x0133;

static constexpr std::array<byte, NINTENDO_LOGO_END - NINTENDO_LOGO_START + 1>
    NINTENDO_LOGO = {0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73,
                     0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F,
                     0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD,
                     0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
                     0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E};

// Memory spaces
static constexpr word MEMORY_FIXED_BANK_START = 0x0000;
static constexpr word MEMORY_FIXED_BANK_END = 0x3FFF;
static constexpr word MEMORY_SWITCH_BANK_START = 0x4000;
static constexpr word MEMORY_SWITCH_BANK_END = 0x7FFF;
static constexpr word MEMORY_VRAM_START = 0x8000;
static constexpr word MEMORY_VRAM_END = 0x9FFF;
static constexpr word MEMORY_ERAM_START = 0xA000;
static constexpr word MEMORY_ERAM_END = 0xBFFF;
static constexpr word MEMORY_WRAM_START = 0xC000;
static constexpr word MEMORY_WRAM_END = 0xCFFF;
static constexpr word MEMORY_SWITCH_WRAM_START = 0xD000;
static constexpr word MEMORY_SWITCH_WRAM_END = 0xDFFF;
static constexpr word MEMORY_HRAM_START = 0xFF80;
static constexpr word MEMORY_HRAM_END = 0xFFFE;

}  // namespace gbdmge

#endif