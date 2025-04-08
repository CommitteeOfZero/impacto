#include "mem.h"

namespace Impacto {

void SetFlag(uint32_t flagId, uint32_t value) {
  if (flagId & 0x80000000) return;

  uint32_t flagIndex = flagId >> 3;
  int flagValue = 1 << (flagId - 8 * (flagId >> 3));
  FlagWork[flagIndex] |= flagValue;
  if (!value) {
    FlagWork[flagIndex] ^= flagValue;
  }
}

bool GetFlag(uint32_t flagId) {
  return ((uint8_t)(1 << (flagId - 8 * (flagId >> 3))) &
          FlagWork[flagId >> 3]) != 0;
}

}  // namespace Impacto