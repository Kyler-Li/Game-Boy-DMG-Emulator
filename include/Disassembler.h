#ifndef GBDMGE_DISASSEMBLER_H
#define GBDMGE_DISASSEMBLER_H

#include <string>
#include "Types.h"

namespace gbdmge {

class Disassembler {
 public:
  static std::string decode(byte opcode, byte byte1 = 0, byte byte2 = 0);

 private:
  static std::string formatHex8(byte value);
  static std::string formatHex16(word value);
  static std::string decodeCB(byte opcode);

  static std::array<const char*, 8> registerNames;
  static std::array<const char*, 4> registerPairNames;
  static std::array<const char*, 4> stackRegisterPairNames;
  static std::array<const char*, 4> conditionNames;
  static std::array<const char*, 8> instructionPrefixes;
  static std::array<const char*, 8> rotateShiftPrefixes;
};
}  // namespace gbdmge

#endif
