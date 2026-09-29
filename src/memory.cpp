#include <stdexcept>
#include "memory.h"

namespace gbdmge {
    Memory::Memory()
    :
    zram {},
    joyp {zram[0x0]},
    sb {zram[0x1]},
    sc {zram[0x2]},
    div {zram[0x4]},
    tima {zram[0x5]},
    tma {zram[0x6]},
    tac {zram[0x7]},
    ie {zram[0xFF]},
    inf {zram[0x0F]},
    nr10 {zram[0x10]},
    nr11 {zram[0x11]},
    nr12 {zram[0x12]},
    nr13 {zram[0x13]},
    nr14 {zram[0x14]},
    nr21 {zram[0x16]},
    nr22 {zram[0x17]},
    nr23 {zram[0x18]},
    nr24 {zram[0x19]},
    nr30 {zram[0x1A]},
    nr31 {zram[0x1B]},
    nr32 {zram[0x1C]},
    nr33 {zram[0x1D]},
    nr34 {zram[0x1E]},
    nr41 {zram[0x20]},
    nr42 {zram[0x21]},
    nr43 {zram[0x22]},
    nr44 {zram[0x23]},
    nr50 {zram[0x24]},
    nr51 {zram[0x25]},
    nr52 {zram[0x26]},
    wavePattern {},
    lcdc {zram[0x40]},
    ly {zram[0x44]},
    lyc {zram[0x45]},
    scy {zram[0x42]},
    scx {zram[0x43]},
    bgp {zram[0x47]},
    obp0 {zram[0x48]},
    obp1 {zram[0x49]},
    stat {zram[0x41]},
    wy {zram[0x4A]},
    wx {zram[0x4B]},
    dma {zram[0x46]},
    boot {zram[0x50]}
    {}

    byte Memory::read(word address) {
        throw std::logic_error("not implemented");
    }

    void Memory::write(word address, byte value) {
        throw std::logic_error("not implemented");
    }

    void Memory::reset() {
        // Initialize registers to post boot ROM values
        throw std::logic_error("not implemented");
    }
}
