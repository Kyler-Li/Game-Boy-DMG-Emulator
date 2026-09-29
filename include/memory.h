#ifndef GBDMGE_MEMORY_H
#define GBDMGE_MEMORY_H

#include "types.h"

namespace gbdmge {
    class Memory{

        friend class CPU;
    public:
        void write(word address, byte value);
        byte read(word address);
    };
}
#endif
