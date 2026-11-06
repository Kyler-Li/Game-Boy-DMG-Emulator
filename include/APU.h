#ifndef GBDMGE_APU_H
#define GBDMGE_APU_H

#include "Types.h"

namespace gbdmge {
class APU
{
public:
    byte read(word address);
    void write(word address, byte value);
    void reset();

private:
    byte nr10, nr11, nr12, nr13, nr14;
    byte nr21, nr22, nr23, nr24;
    byte nr30, nr31, nr32, nr33, nr34;
    byte nr41, nr42, nr43, nr44;
    byte nr50, nr51, nr52;
};
}

#endif
