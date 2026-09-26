#pragma once

#include "types.h"

class Memory{


public:
    byte read(word address);
    void write(word address, byte data);
};