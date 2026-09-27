#pragma once

#include "types.h"

class Memory{

public:
    void write(word address, byte value);
    byte read(word address);
};