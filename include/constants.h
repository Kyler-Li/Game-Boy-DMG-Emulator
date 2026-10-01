#ifndef GBDMGE_ENUMLATOR_H
#define GBDMGE_ENUMLATOR_H

#include "types.h"

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

// CPU flags
static constexpr byte CPU_ZERO_FLAG = 7;
static constexpr byte CPU_SUB_FLAG = 6;
static constexpr byte CPU_HALF_CARRY_FLAG = 5;
static constexpr byte CPU_CARRY_FLAG = 4;

}  // namespace gbdmge

#endif