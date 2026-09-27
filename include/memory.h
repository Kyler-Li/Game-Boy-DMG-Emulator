#ifndef GBDMGE_MEMORY_H_
#define GBDMGE_MEMORY_H_

#include "types.h"

namespace gbdmge {
    class Memory{

    public:
        void write(word address, byte value);
        byte read(word address);
    };
}
#endif
