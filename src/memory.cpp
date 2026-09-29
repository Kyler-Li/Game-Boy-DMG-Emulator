#include <stdexcept>
#include "memory.h"

namespace gbdmge {
    Memory::Memory()
    :
    vram {},
    wram {},
    oram {},
    ioram {},
    p1 {ioram[0x0]},
    sb {ioram[0x1]},
    sc {ioram[0x2]},
    div {ioram[0x4]},
    tima {ioram[0x5]},
    tma {ioram[0x6]},
    tac {ioram[0x7]},
    ie {ioram[0xFF]},
    inf {ioram[0x0F]},
    nr10 {ioram[0x10]},
    nr11 {ioram[0x11]},
    nr12 {ioram[0x12]},
    nr13 {ioram[0x13]},
    nr14 {ioram[0x14]},
    nr21 {ioram[0x16]},
    nr22 {ioram[0x17]},
    nr23 {ioram[0x18]},
    nr24 {ioram[0x19]},
    nr30 {ioram[0x1A]},
    nr31 {ioram[0x1B]},
    nr32 {ioram[0x1C]},
    nr33 {ioram[0x1D]},
    nr34 {ioram[0x1E]},
    nr41 {ioram[0x20]},
    nr42 {ioram[0x21]},
    nr43 {ioram[0x22]},
    nr44 {ioram[0x23]},
    nr50 {ioram[0x24]},
    nr51 {ioram[0x25]},
    nr52 {ioram[0x26]},
    wavePattern {},
    lcdc {ioram[0x40]},
    ly {ioram[0x44]},
    lyc {ioram[0x45]},
    scy {ioram[0x42]},
    scx {ioram[0x43]},
    bgp {ioram[0x47]},
    obp0 {ioram[0x48]},
    obp1 {ioram[0x49]},
    stat {ioram[0x41]},
    wy {ioram[0x4A]},
    wx {ioram[0x4B]},
    dma {ioram[0x46]}
    {}

    // Need to add support for ROM banks and external RAM
    byte Memory::read(const word address) const {
        switch (address & 0xF000) {
            case 0x0000:
            case 0x1000:
            case 0x2000:
            case 0x3000:
            case 0x4000:
            case 0x5000:
            case 0x6000:
            case 0x7000:
                throw std::logic_error("reading from rom banks not implemented");

            case 0x8000:
            case 0x9000:
                return vram[address & VW_RANGE_MASK];

            case 0xA000:
            case 0xB000:
                // External RAM
                throw std::logic_error("reading from external ram not implemented");

            case 0xC000:
            case 0xD000:
            case 0xE000:
            case 0xF000:
                if (address < 0xFE00) {
                    return wram[address & VW_RANGE_MASK]; // Work RAM and echo RAM. Echo RAM is a mirror of work RAM
                }

                if (address < 0xFEA0) {
                    return oram[address & OZ_RANGE_MASK];
                }

                if (address < 0xFF00) {
                    throw std::logic_error("reading from unusable memory");
                }

                if (address >= 0xFF30 && address < 0xFF40) {
                    return wavePattern[address & WAVE_RANGE_MASK];
                }

                return ioram[address & OZ_RANGE_MASK]; // I/O, HRAM, IE
            default:
                throw std::logic_error("error, unmatched address");
        }
    }

    void Memory::write(word address, byte value) {
        throw std::logic_error("not implemented");
    }

    void Memory::reset() {
        // Initialize registers to post boot ROM values
        vram.fill(0);
        wram.fill(0);
        oram.fill(0);
        ioram.fill(0);
        wavePattern.fill(0);

        p1.set(0xCF);
        sb.set(0x00);
        sc.set(0x7E);
        div.set(0xAB);
        tima.set(0x00);
        tma.set(0x00);
        tac.set(0xF8);
        inf.set(0xE1);
        nr10.set(0x80);
        nr11.set(0xBF);
        nr12.set(0xF3);
        nr13.set(0xFF);
        nr14.set(0xBF);
        nr21.set(0x3F);
        nr22.set(0x00);
        nr23.set(0xFF);
        nr24.set(0xBF);
        nr30.set(0x7F);
        nr31.set(0xFF);
        nr32.set(0x9F);
        nr33.set(0xFF);
        nr34.set(0xBF);
        nr41.set(0xFF);
        nr42.set(0x00);
        nr43.set(0x00);
        nr44.set(0xBF);
        nr50.set(0x77);
        nr51.set(0xF3);
        nr52.set(0xF1);
        lcdc.set(0x91);
        stat.set(0x85);
        scy.set(0x00);
        scx.set(0x00);
        ly.set(0x00);
        lyc.set(0x00);
        dma.set(0xFF);
        bgp.set(0xFC);
        obp0.set(0xFF); // Documentation says 0x00 or 0xFF, change later?
        obp1.set(0xFF); // Documentation says 0x00 or 0xFF, change later?
        wy.set(0x00);
        wx.set(0x00);
        ie.set(0x00);
    }
}
