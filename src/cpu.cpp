#include "cpu.h"

#include <stdexcept>

namespace gbdmge {
    CPU::CPU(Memory& memory)
    :
    ime {false}, // Interrupts are disabled on boot
    memory {memory},
    tCycles {0}
    {}

    void CPU::pop(Reg& src) {
        const word address = sp.get();
        src.setLow(memory.read(address));
        src.setHigh(memory.read(address + 1));
        sp.inc();
        sp.inc();
    }

    void CPU::push(Reg& src) {
        const word address = sp.get();
        memory.write(address - 1, src.getHigh());
        memory.write(address - 2, src.getLow());
        sp.dec();
        sp.dec();
    }

    void CPU::anda(const byte src) {
        af.setHigh(af.getHigh() & src);
        af.setLow(0);

        byte& low = af.getLow();
        af.setBit(low, ZERO_FLAG, af.getHigh() == 0);
        af.setBit(low, HALF_CARRY_FLAG, true);
    }

    void CPU::ora(const byte src) {
        af.setHigh(af.getHigh() | src);
        af.setLow(0);
        af.setBit(af.getLow(), ZERO_FLAG, af.getHigh() == 0);
    }

    void CPU::xora(const byte src) {
        af.setHigh(af.getHigh() ^ src);
        af.setLow(0);
        af.setBit(af.getLow(), ZERO_FLAG, af.getHigh() == 0);
    }

    void CPU::cp(const byte b) {
        const byte a = af.getHigh();

        byte& low = af.getLow();
        af.setBit(low, ZERO_FLAG, a == b);
        af.setBit(low, SUB_FLAG, true);
        af.setBit(low, HALF_CARRY_FLAG, (a & 0x0F) < (b & 0x0F));
        af.setBit(low, CARRY_FLAG, a < b);
    }

    void CPU::cp(Reg& b) {
        cp(memory.read(b.get()));
    }

    void CPU::jpz(word address) {
        jpCond(address, af.bitIsSet(af.getLow(), ZERO_FLAG));
    }

    void CPU::jpnz(const word address) {
        jpCond(address, !af.bitIsSet(af.getLow(), ZERO_FLAG));
    }

    void CPU::jpc(const word address) {
        jpCond(address, af.bitIsSet(af.getLow(), CARRY_FLAG));
    }

    void CPU::jpnc(const word address) {
        jpCond(address, !af.bitIsSet(af.getLow(), CARRY_FLAG));
    }

    void CPU::jp(Reg& dst) {
        jpCond(dst.get(), true);
    }

    void CPU::jp(const word address) {
        jpCond(address, true);
    }

    void CPU::jpCond(const word address, const bool cond) {
        if (cond) {
            pc.set(address);
        }
    }

    void CPU::nop() {

    }

    void CPU::cpl() {
        af.setHigh(~af.getHigh());
        byte& low = af.getLow();
        af.setBit(low, HALF_CARRY_FLAG, true);
        af.setBit(low, SUB_FLAG, true);
    }

    void CPU::ei() {
        ime = true;
    }

    void CPU::di() {
        ime = false;
    }

    void CPU::reset() {
        // Initialize to post boot ROM values
        ime = false;
        af.set(0x01B0);
        bc.set(0x0013);
        de.set(0x00D8);
        hl.set(0x014D);
        sp.set(0xFFFE);
        pc.set(0x0100);
    }

    byte CPU::fetch() {
        const word address = pc.get();
        pc.inc();
        return memory.read(address);
    }

    // Returns the number of T-cycles after execution
    qword CPU::execute(byte opcode) {
        throw std::logic_error("not implemented");
    }
}
