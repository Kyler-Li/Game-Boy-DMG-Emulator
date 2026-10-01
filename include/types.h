#ifndef GBDMGE_TYPES_H
#define GBDMGE_TYPES_H

#include <cstdint>

namespace gbdmge {
using byte = std::uint8_t;
using sbyte = std::int8_t;
using word = std::uint16_t;
using sword = std::int16_t;
using dword = std::uint32_t;
using sdword = std::int32_t;
using qword = std::uint64_t;

// Masks upper bits
static constexpr word MASK_4_UPPER_WORD = 0xF000;

// Masks lower bits
static constexpr byte MASK_1_LOWER_BYTE = 0x01;
static constexpr byte MASK_2_LOWER_BYTE = 0x03;
static constexpr byte MASK_4_LOWER_BYTE = 0x0F;
static constexpr byte MASK_5_LOWER_BYTE = 0x1F;
static constexpr byte MASK_8_LOWER_BYTE = 0xFF;
static constexpr word MASK_13_LOWER_WORD = 0x1FFF;

// Mask for specific bits
static constexpr byte MASK_5_6_BYTE = 0x60;

template <typename T>
class BaseRegister {
 public:
  virtual ~BaseRegister() = default;

  void setBit(byte& value, const byte pos, const bool set) {
    const byte mask = static_cast<byte>(1) << pos;
    if (set) {
      value |= mask;
    } else {
      value &= ~mask;
    }
  }

  bool bitIsSet(const byte& value, const byte pos) {
    return (value & static_cast<byte>(1) << pos) != 0;
  }

  virtual T get() = 0;
  virtual void set(T value) = 0;
  virtual void clear() = 0;
  virtual void inc() = 0;
  virtual void dec() = 0;
};

class Register : public BaseRegister<word> {
  byte low;
  byte high;

 public:
  Register();

  word get() override;
  void set(word value) override;
  void clear() override;
  void inc() override;
  void dec() override;

  [[nodiscard]] byte& getLow();
  [[nodiscard]] byte& getHigh();

  void setLow(byte value);
  void setHigh(byte value);
};

class MemoryRegister : public BaseRegister<byte> {
  byte& data;

 public:
  explicit MemoryRegister(byte& value);

  byte get() override;
  void set(byte value) override;
  void clear() override;
  void inc() override;
  void dec() override;
};

}  // namespace gbdmge

#endif