#ifndef GBDMGE_MEMORY_H
#define GBDMGE_MEMORY_H

#include <memory>
#include <vector>
#include "types.h"

namespace gbdmge {
    /*
    * Memory Bank Controller (MBC):
    *
    * The Game Boy CPU has a 16-bit address bus, giving it only 64 KiB of
    * addressable memory at one time. Cartridges can contain much more ROM
    * and RAM than can fit into that address space, so the cartridge uses an
    * MBC to control which portion of the cartridge is visible to the CPU.
    *
    * The CPU sees the cartridge ROM through two main address ranges:
    *
    *   0000-3FFF -> fixed 16 KiB ROM region
    *   4000-7FFF -> switchable 16 KiB ROM region
    *
    * The MBC keeps track of which ROM bank is selected for the switchable
    * region. When the CPU reads an address in 4000-7FFF, the MBC translates
    * that CPU address into an address within the currently selected physical
    * ROM bank.
    *
    * The CPU also accesses cartridge external RAM through:
    *
    *   A000-BFFF -> 8 KiB external RAM region
    *
    * If the cartridge contains more RAM than 8 KiB, the MBC can similarly
    * select which RAM bank is mapped into this address range.
    *
    * Writes to certain cartridge address ranges are not actually writes to
    * ROM. Instead, they are commands to the MBC, such as enabling external
    * RAM or selecting a ROM/RAM bank. The exact behavior depends on the type
    * of MBC used by the cartridge.
    *
    * In short:
    *
    *   CPU address -> MBC translates it -> physical cartridge ROM/RAM
    *
    * Different MBC types (MBC1, MBC2, MBC3, MBC5, etc.) mainly differ in
    * how they interpret these commands and how they perform the bank mapping.
    */
    class MemoryBC {
        std::vector<byte> rom;
        std::vector<byte> eram;
    public:
        explicit MemoryBC(const std::vector<byte>& romData);
        virtual void write(word address, byte value) = 0;
        virtual byte read(word address) = 0;
        virtual ~MemoryBC() = default;
    };

    class Memory{
        std::unique_ptr<MemoryBC> memoryBC;

        std::vector<byte> vram; // Video ram, 8000 - 9FFF
        std::vector<byte> wram; // Work ram, C000 - DFFF

        std::vector<byte> oram; // FE00 - FE9F
        std::vector<byte> ioram; // FF00 - FFFF

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

        std::vector<byte> wavePattern; // Wave pattern

        MemoryReg lcdc, ly, lyc, scy, scx, bgp, obp0, obp1; // LCD control
        MemoryReg stat, wy, wx; // LCD control
        MemoryReg dma; // Direct memory access
    public:
        Memory();
        void write(word address, byte value);
        [[nodiscard]] byte read(word address) const;
        void loadRom(const std::string& romFileName);
        void reset();
    };
}
#endif
