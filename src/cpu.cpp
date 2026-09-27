#include "cpu.h"

#include <exception>
#include <stdexcept>

namespace gbdmge {
    CPU::CPU(Memory& memory) : memory {memory}, tCycles {0} {}

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

    void CPU::anda(const byte& src) {
        af.setHigh(af.getHigh() & src);
        af.setLow(0);

        byte& low = af.getLow();
        af.setBit(low, ZERO_FLAG, af.getHigh() == 0);
        af.setBit(low, HALF_CARRY_FLAG, true);
    }

    void CPU::ora(const byte& src) {
        af.setHigh(af.getHigh() | src);
        af.setLow(0);
        af.setBit(af.getLow(), ZERO_FLAG, af.getHigh() == 0);
    }

    void CPU::xora(const byte& src) {
        af.setHigh(af.getHigh() ^ src);
        af.setLow(0);
        af.setBit(af.getLow(), ZERO_FLAG, af.getHigh() == 0);
    }

    void CPU::reset() {
        // Initialize registers to post boot ROM values
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
