#include "Disassembler.h"

#include <array>
#include <iomanip>
#include <sstream>

namespace gbdmge {
std::array<const char*, 8> Disassembler::registerNames = {
    "B", "C", "D", "E", "H", "L", "(HL)", "A"};
std::array<const char*, 4> Disassembler::registerPairNames = {"BC", "DE", "HL",
                                                              "SP"};
std::array<const char*, 4> Disassembler::stackRegisterPairNames = {"BC", "DE",
                                                                   "HL", "AF"};
std::array<const char*, 4> Disassembler::conditionNames = {"NZ", "Z", "NC",
                                                           "C"};
std::array<const char*, 8> Disassembler::instructionPrefixes = {
    "ADD A,", "ADC A,", "SUB ", "SBC A,", "AND ", "XOR ", "OR ", "CP "};
std::array<const char*, 8> rotateShift = {"RLC ", "RRC ", "RL ",   "RR ",
                                          "SLA ", "SRA ", "SWAP ", "SRL "};

std::array<const char*, 8> Disassembler::rotateShiftPrefixes = {
    "RLC ", "RRC ", "RL ", "RR ", "SLA ", "SRA ", "SWAP ", "SRL "};

std::string Disassembler::formatHex8(const byte value) {
  std::ostringstream ss;
  ss << "$" << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
     << static_cast<int>(value);
  return ss.str();
}

std::string Disassembler::formatHex16(const word value) {
  std::ostringstream ss;
  ss << "$" << std::uppercase << std::hex << std::setw(4) << std::setfill('0')
     << static_cast<int>(value);
  return ss.str();
}

std::string Disassembler::decodeCB(const byte opcode) {
  const unsigned operationGroup = opcode >> 6;
  const unsigned operationIndex = opcode >> 3 & 7;
  const unsigned registerIndex = opcode & 7;

  const std::string reg = registerNames[registerIndex];

  if (operationGroup == 0) {
    return std::string(rotateShiftPrefixes[operationIndex]) + reg;
  }

  if (operationGroup == 1) {
    return "BIT " + std::to_string(operationIndex) + "," + reg;
  }

  if (operationGroup == 2) {
    return "RES " + std::to_string(operationIndex) + "," + reg;
  }

  return "SET " + std::to_string(operationIndex) + "," + reg;
}

std::string Disassembler::decode(const byte opcode, const byte byte1,
                                 const byte byte2) {
  const std::string immediate8{formatHex8(byte1)};
  const std::string immediate16{
      formatHex16(static_cast<word>(byte1) | static_cast<word>(byte2) << 8)};

  // CB-prefixed instructions.
  if (opcode == 0xCB) {
    return decodeCB(byte1);
  }

  // LD r,r and HALT.
  if (opcode >= 0x40 && opcode <= 0x7F) {
    if (opcode == 0x76) {
      return {"HALT"};
    }

    return std::string{"LD "} + registerNames[(opcode >> 3) & 7] + "," +
           registerNames[opcode & 7];
  }

  // ALU operations on registers.
  if (opcode >= 0x80 && opcode <= 0xBF) {
    return std::string{instructionPrefixes[(opcode >> 3) & 7]} +
           registerNames[opcode & 7];
  }

  // LD rr,d16
  if ((opcode & 0xCF) == 0x01) {
    return std::string{"LD "} + registerPairNames[(opcode >> 4) & 3] + "," +
           immediate16;
  }

  // INC rr
  if ((opcode & 0xCF) == 0x03) {
    return std::string{"INC "} + registerPairNames[(opcode >> 4) & 3];
  }

  // DEC rr
  if ((opcode & 0xCF) == 0x0B) {
    return std::string{"DEC "} + registerPairNames[(opcode >> 4) & 3];
  }

  // ADD HL,rr
  if ((opcode & 0xCF) == 0x09) {
    return std::string{"ADD HL,"} + registerPairNames[(opcode >> 4) & 3];
  }

  // INC r
  if ((opcode & 0xC7) == 0x04) {
    return std::string{"INC "} + registerNames[(opcode >> 3) & 7];
  }

  // DEC r
  if ((opcode & 0xC7) == 0x05) {
    return std::string{"DEC "} + registerNames[(opcode >> 3) & 7];
  }

  // LD r,d8
  if ((opcode & 0xC7) == 0x06) {
    return std::string{"LD "} + registerNames[(opcode >> 3) & 7] + "," +
           immediate8;
  }

  // Conditional RET
  if ((opcode & 0xE7) == 0xC0) {
    return std::string{"RET "} + conditionNames[(opcode >> 3) & 3];
  }

  // POP rr
  if ((opcode & 0xCF) == 0xC1) {
    return std::string{"POP "} + stackRegisterPairNames[(opcode >> 4) & 3];
  }

  // Conditional JP
  if ((opcode & 0xE7) == 0xC2) {
    return std::string{"JP "} + conditionNames[(opcode >> 3) & 3] + "," +
           immediate16;
  }

  // Conditional CALL
  if ((opcode & 0xE7) == 0xC4) {
    return std::string{"CALL "} + conditionNames[(opcode >> 3) & 3] + "," +
           immediate16;
  }

  // PUSH rr
  if ((opcode & 0xCF) == 0xC5) {
    return std::string{"PUSH "} + stackRegisterPairNames[(opcode >> 4) & 3];
  }

  // Immediate ALU instructions
  if ((opcode & 0xC7) == 0xC6) {
    return std::string{instructionPrefixes[(opcode >> 3) & 7]} + immediate8;
  }

  // RST vectors
  if ((opcode & 0xC7) == 0xC7) {
    return std::string{"RST "} + formatHex8(opcode & 0x38);
  }

  switch (opcode) {
    case 0x00:
      return {"NOP"};
    case 0x02:
      return {"LD (BC),A"};
    case 0x07:
      return {"RLCA"};
    case 0x08:
      return std::string{"LD ("} + immediate16 + "),SP";
    case 0x0A:
      return {"LD A,(BC)"};
    case 0x0F:
      return {"RRCA"};
    case 0x10:
      return {"STOP"};
    case 0x12:
      return {"LD (DE),A"};
    case 0x17:
      return {"RLA"};
    case 0x1A:
      return {"LD A,(DE)"};
    case 0x1F:
      return {"RRA"};
    case 0x18:
      return std::string{"JR "} + immediate8;
    case 0x20:
      return std::string{"JR NZ,"} + immediate8;
    case 0x28:
      return std::string{"JR Z,"} + immediate8;
    case 0x30:
      return std::string{"JR NC,"} + immediate8;
    case 0x38:
      return std::string{"JR C,"} + immediate8;
    case 0x22:
      return {"LDI (HL),A"};
    case 0x27:
      return {"DAA"};
    case 0x2A:
      return {"LDI A,(HL)"};
    case 0x2F:
      return {"CPL"};
    case 0x32:
      return {"LDD (HL),A"};
    case 0x37:
      return {"SCF"};
    case 0x3A:
      return {"LDD A,(HL)"};
    case 0x3F:
      return {"CCF"};
    case 0xC3:
      return std::string{"JP "} + immediate16;
    case 0xC9:
      return {"RET"};
    case 0xCD:
      return std::string{"CALL "} + immediate16;
    case 0xD9:
      return {"RETI"};
    case 0xE0:
      return std::string{"LDH ("} + immediate8 + "),A";
    case 0xE2:
      return {"LD (C),A"};
    case 0xE8:
      return std::string{"ADD SP,"} + immediate8;
    case 0xE9:
      return {"JP (HL)"};
    case 0xEA:
      return std::string{"LD ("} + immediate16 + "),A";
    case 0xF0:
      return std::string{"LDH A,("} + immediate8 + ")";
    case 0xF2:
      return {"LD A,(C)"};
    case 0xF3:
      return {"DI"};
    case 0xF8:
      return std::string{"LD HL,SP+"} + immediate8;
    case 0xF9:
      return {"LD SP,HL"};
    case 0xFA:
      return std::string{"LD A,("} + immediate16 + ")";
    case 0xFB:
      return {"EI"};
    case 0xD3:
    case 0xDB:
    case 0xDD:
    case 0xE3:
    case 0xE4:
    case 0xEB:
    case 0xEC:
    case 0xED:
    case 0xF4:
    case 0xFC:
    case 0xFD:
      return std::string{"ILLEGAL "} + formatHex8(opcode);
    default:
      return std::string{"UNKNOWN "} + formatHex8(opcode);
  }
}
}  // namespace gbdmge
