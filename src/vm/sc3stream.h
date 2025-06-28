#pragma once

#include <cstdint>
#include <span>
#include "thread.h"

namespace Impacto {
namespace Vm {

class Sc3Stream {
 public:
  Sc3Stream(uint16_t* ptr) : Ptr(reinterpret_cast<uint8_t*>(ptr)) {}
  Sc3Stream(uint8_t* ptr) : Ptr(ptr) {}
  uint8_t ReadU8() {
    uint8_t result = PeekU8();
    Advance(1);
    return result;
  }

  uint16_t ReadU16() {
    uint16_t result = PeekU16();
    Advance(2);
    return result;
  }
  uint16_t PeekU16(int64_t offset = 0) const {
    auto adjOffset = offset * sizeof(uint16_t);
    return (PeekU8(adjOffset + 1) << 8) | PeekU8(adjOffset);
  }
  uint8_t PeekU8(int64_t offset = 0) const { return *(Ptr + offset); };
  uint8_t* Data() const { return Ptr; }
  void Advance(int64_t n) { Ptr += n; }

 private:
  uint8_t* Ptr{};
};
}  // namespace Vm
}  // namespace Impacto