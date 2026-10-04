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

 private:
  byte low;
  byte high;
};

class MemoryRegister : public BaseRegister<byte> {
 public:
  explicit MemoryRegister(byte& value);

  byte get() override;
  void set(byte value) override;
  void clear() override;
  void inc() override;
  void dec() override;

 private:
  byte& data;
};

}  // namespace gbdmge

#endif