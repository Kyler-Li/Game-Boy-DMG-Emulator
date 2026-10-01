#ifndef GBDMGE_MEMORY_H
#define GBDMGE_MEMORY_H

#include <memory>
#include <vector>
#include "types.h"

namespace gbdmge {
    class MemoryBC {
    protected:
        std::vector<byte> rom;
        std::vector<byte> eram;

        bool eRamEnabled;
        byte bankingNumber;
        byte bankingMode;
    public:
        explicit MemoryBC(const std::vector<byte>& romData);
        virtual void write(word address, byte value) = 0;
        virtual byte read(word address) = 0;
        virtual ~MemoryBC() = default;
    };

    class MemoryBC0 : public MemoryBC {
    public:
        explicit MemoryBC0(const std::vector<byte>& romData);
        void write(word address, byte value) override;
        byte read(word address) override;
        ~MemoryBC0() override = default;
    };

    class MemoryBC1 : public MemoryBC {
        /*
            Here is the expected behavior of MBC1:

            Reading:
            if bankingMode = 1, RAM mode (up to 32KByte RAM, 512KByte ROM)
            Reading from 0000 - 3FFF, simply reads a byte.
            Reading from A000 - 0BFFF, use bits 5,6 of bankingNumber to select eram bank for eram reads.
            Reading from 4000 - 7FFF, use bits 0-4 of bankingNumber to select rom bank for rom reads.

            if bankingMode = 0, ROM mode (up to 8KByte RAM, 2MByte ROM) (default)
            Reading from 0000 - 3FFF, simply reads a byte.
            Reading from A000 - 0BFFF, use eram bank 0 for eram reads.
            Reading from 4000 - 7FFF, use the entire bankingNumber to select rom bank for rom reads.

            Writing:
            Writing to 0000 - 1FFF enables eram writes and reads if the value written has 0x0A in the lower 4 bits, otherwise disables eram writes and reads.
            Writing to 2000 - 3FFF sets the lower 5 bits of the bankingNumber. If the resulting bankingNumber is 0x0, 0x20, 0x40 or 0x60,
            it is incremented.
            Writing to 4000 - 5FFF sets bits 5, 6 of the bankingNumber. If the resulting bankingNumber is 0x0, 0x20, 0x40 or 0x60,
            it is incremented.
            Writing to 6000 - 7FFF sets the bankingMode. If the value is 1, RAM mode is selected
            if the value is 0, ROM mode is selected.
         */
        void incrementBankingNumber();
    public:
        explicit MemoryBC1(const std::vector<byte>& romData);
        void write(word address, byte value) override;
        byte read(word address) override;
        ~MemoryBC1() override = default;
    };

    class Memory{
        std::unique_ptr<MemoryBC> memoryBC; // Manages reads and writes to 0000-7FFF, A000-BFFF

        std::vector<byte> vram; // Video ram, 8000 - 9FFF
        std::vector<byte> wram; // Work ram, C000 - DFFF

        std::vector<byte> oram; // FE00 - FE9F
        std::vector<byte> ioram; // FF00 - FFFF

        // IO Registers, which are mapped to addresses in range FF00 - FFFF
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
