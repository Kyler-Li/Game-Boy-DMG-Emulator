#include "bus.h"

namespace gbdmge {
Bus::Bus(Console& console) : console{&console} {}

auto Bus::read(word address) -> byte& {
  // TODO: Implement
}

void Bus::write(word address, byte value) {
  // TODO: Implement
}

}  // namespace gbdmge