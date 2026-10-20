#ifndef GBDMGE_BUS_H
#define GBDMGE_BUS_H

#include "Types.h"

namespace gbdmge {
class Console;

//! @brief Intercepts external read and write requests and reroutes them across
//! different components. This allows different components to respond accordingly
//! for requests targeted for their respective memory ranges.
class Bus {
 public:
  explicit Bus(Console& console);
  auto read(word address) const -> byte;
  void write(word address, byte value);

 private:
  Console* console{};
};
}  // namespace gbdmge

#endif