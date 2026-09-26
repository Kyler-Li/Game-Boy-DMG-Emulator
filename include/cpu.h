#pragma once

#include "types.h"
#include "memory.h"

class CPU{
    Memory& memory;
    RegPair regAF;
    RegPair regBC;
    RegPair regDE;
    RegPair regHL;
    word SP;
    word PC;
public:
    CPU(Memory& memory) : memory(memory) {}
    void reset();
};