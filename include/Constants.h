#ifndef GBDMGE_ENUMLATOR_H
#define GBDMGE_ENUMLATOR_H

#include <array>
#include "Types.h"

namespace gbdmge {
// Masks upper bits from MSB to LSB
static constexpr byte MASK_1_UPPER_BYTE = 0x80;
static constexpr word MASK_4_UPPER_WORD = 0xF000;
static constexpr byte MASK_4_UPPER_BYTE = 0xF0;
static constexpr byte MASK_8_UPPER_BYTE = 0xFE;

// Masks lower bits from LSB to MSB
static constexpr byte MASK_1_LOWER_BYTE = 0x01;
static constexpr word MASK_1_LOWER_WORD = 0x0001;
static constexpr byte MASK_2_LOWER_BYTE = 0x03;
static constexpr byte MASK_3_LOWER_BYTE = 0x07;
static constexpr byte MASK_4_LOWER_BYTE = 0x0F;
static constexpr word MASK_4_LOWER_WORD = 0x000F;
static constexpr byte MASK_5_LOWER_BYTE = 0x1F;
static constexpr byte MASK_6_LOWER_BYTE = 0x3F;
static constexpr byte MASK_7_LOWER_BYTE = 0x7F;
static constexpr byte MASK_8_LOWER_BYTE = 0xFF;
static constexpr word MASK_8_LOWER_WORD = 0x00FF;
static constexpr word MASK_12_LOWER_WORD = 0x0FFF;
static constexpr word MASK_13_LOWER_WORD = 0x1FFF;
static constexpr word MASK_16_LOWER_WORD = 0x0000FFFF;

// Mask for specific bits
static constexpr byte MASK_5_6_BYTE = 0x60;

// Indexes for specific bits
static constexpr byte BIT_0_BYTE = 0;
static constexpr byte BIT_7_BYTE = 7;

// Constants for Binary Coded Decimals
static constexpr byte BCD_PLACE_MAXIMUM_BYTE = 0x09;
static constexpr byte BCD_MAXIMUM_BYTE = 0x99;
static constexpr byte BCD_ONES_PLACE_ADJUST_BYTE = 0x06;
static constexpr byte BCD_TENS_PLACE_ADJUST_BYTE = 0x60;

// Positions for flags register in CPU
// Flags register layout:
// Bit:     |   7      |      6      |     5          |   4        |  3 - 0  |
// Meaning: | Zero (Z) | Subtraction | Half Carry (H) | Carry (CY) |  Unused |
static constexpr byte CPU_ZERO_FLAG = 7;
static constexpr byte CPU_SUB_FLAG = 6;
static constexpr byte CPU_HALF_CARRY_FLAG = 5;
static constexpr byte CPU_CARRY_FLAG = 4;

// Positions for flags in IE and IF IO registers
static constexpr byte INTERRUPT_VBLANK_FLAG = 0;
static constexpr byte INTERRUPT_LCD_FLAG = 1;
static constexpr byte INTERRUPT_TIMER_FLAG = 2;
static constexpr byte INTERRUPT_SERIAL_FLAG = 3;
static constexpr byte INTERRUPT_JOYPAD_FLAG = 4;

// Addresses for interrupt handlers
static constexpr word MEMORY_INTERRUPT_VBLANK = 0x0040;
static constexpr word MEMORY_INTERRUPT_LCD = 0x0048;
static constexpr word MEMORY_INTERRUPT_TIMER = 0x0050;
static constexpr word MEMORY_INTERRUPT_SERIAL = 0x0058;
static constexpr word MEMORY_INTERRUPT_JOYPAD = 0x0060;

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
static constexpr word MEMORY_LCD_START = 0xFF40;
static constexpr word MEMORY_LCD_END = 0xFF4B;
static constexpr word MEMORY_ERAM_START = 0xA000;
static constexpr word MEMORY_ERAM_END = 0xBFFF;
static constexpr word MEMORY_WRAM_START = 0xC000;
static constexpr word MEMORY_WRAM_END = 0xDFFF;
static constexpr word MEMORY_ECHO_RAM_START = 0xE000;
static constexpr word MEMORY_ECHO_RAM_END = 0xFDFF;
static constexpr word MEMORY_OAM_START = 0xFE00;
static constexpr word MEMORY_OAM_END = 0xFE9F;
static constexpr word MEMORY_FORBIDDEN_START = 0xFEA0;
static constexpr word MEMORY_FORBIDDEN_END = 0xFEFF;
static constexpr word MEMORY_TIMER_START = 0xFF04;
static constexpr word MEMORY_TIMER_END = 0xFF07;
static constexpr word MEMORY_AUDIO_START = 0xFF10;
static constexpr word MEMORY_AUDIO_END = 0xFF3F;
static constexpr word MEMORY_HRAM_START = 0xFF80;
static constexpr word MEMORY_HRAM_END = 0xFFFE;
static constexpr word MEMORY_OAM_DMA = 0xFF46;

static constexpr word CARTRIDGE_ROM_BANK_SIZE = 0x4000;
static constexpr word CARTRIDGE_ERAM_BANK_SIZE = 0x2000;
static constexpr word CARTRIDGE_ERAM_BANK_NUMBER_START = 0x4000;
static constexpr word CARTRIDGE_ERAM_BANK_NUMBER_END = 0x5FFF;
static constexpr word CARTRIDGE_ROM_BANK_NUMBER_START = 0x2000;
static constexpr word CARTRIDGE_ROM_BANK_NUMBER_END = 0x3FFF;
static constexpr word CARTRIDGE_ROM_ERAM_MODE_SELECT_START = 0x6000;
static constexpr word CARTRIDGE_ROM_ERAM_MODE_SELECT_END = 0x7FFF;
static constexpr byte CARTRIDGE_ERAM_ENABLE_VALUE = 0x0A;
static constexpr word CARTRIDGE_ERAM_ENABLE_END = 0x1FFF;
static constexpr byte CARTRIDGE_ERAM_BANKING_MODE = 1;
static constexpr byte CARTRIDGE_ROM_BANKING_MODE = 0;
static constexpr word CARTRIDGE_TYPE_START = 0x147;
static constexpr word CARTRIDGE_ROM_SIZE_VALUE = 0x148;
static constexpr word CARTRIDGE_ERAM_SIZE_START = 0x149;

// IO Registers
static constexpr word MEMORY_IO_JOYPAD_INPUT = 0xFF00;
static constexpr word MEMORY_IO_SERIAL_TRANSFER_SB = 0xFF01;
static constexpr word MEMORY_IO_SERIAL_TRANSFER_SC = 0xFF02;
static constexpr word MEMORY_IO_IF = 0xFF0F;
static constexpr word MEMORY_IO_NR10 = 0xFF10;
static constexpr word MEMORY_IO_NR11 = 0xFF11;
static constexpr word MEMORY_IO_NR12 = 0xFF12;
static constexpr word MEMORY_IO_NR13 = 0xFF13;
static constexpr word MEMORY_IO_NR14 = 0xFF14;
static constexpr word MEMORY_IO_NR21 = 0xFF16;
static constexpr word MEMORY_IO_NR22 = 0xFF17;
static constexpr word MEMORY_IO_NR23 = 0xFF18;
static constexpr word MEMORY_IO_NR24 = 0xFF19;
static constexpr word MEMORY_IO_NR30 = 0xFF1A;
static constexpr word MEMORY_IO_NR31 = 0xFF1B;
static constexpr word MEMORY_IO_NR32 = 0xFF1C;
static constexpr word MEMORY_IO_NR33 = 0xFF1D;
static constexpr word MEMORY_IO_NR34 = 0xFF1E;
static constexpr word MEMORY_IO_NR41 = 0xFF20;
static constexpr word MEMORY_IO_NR42 = 0xFF21;
static constexpr word MEMORY_IO_NR43 = 0xFF22;
static constexpr word MEMORY_IO_NR44 = 0xFF23;
static constexpr word MEMORY_IO_NR50 = 0xFF24;
static constexpr word MEMORY_IO_NR51 = 0xFF25;
static constexpr word MEMORY_IO_NR52 = 0xFF26;
static constexpr word MEMORY_IO_IE = 0xFFFF;

}  // namespace gbdmge

#endif
