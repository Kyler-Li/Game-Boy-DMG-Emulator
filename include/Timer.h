#ifndef GBDMGE_TIMER_H
#define GBDMGE_TIMER_H

#include "Types.h"

namespace gbdmge {

class Timer {
 public:
  Timer() = default;
  byte read(word address);
  void write(word address, byte value);
  void reset();  // Initialize to post boot rom state
};

}  // namespace gbdmge

#endif
