#include <stdexcept>
#include "memory.h"

namespace gbdmge {
    byte Memory::read(word address) {
        throw std::logic_error("not implemented");
    }

    void Memory::write(word address, byte value) {
        throw std::logic_error("not implemented");

    }
}
