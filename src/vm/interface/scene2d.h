#pragma once
#include "../../impacto.h"
#include "../../background2d.h"
#include "../../character2d.h"

namespace Impacto {
namespace Vm {
namespace Interface {

void UpdateBackground2D();

void UpdateCharacter2D();

void UpdateSpeakerPortraits();

// Because yes
inline int32_t GetBufferId(int32_t bufIdByScript) {
#ifdef PLATFORM_DREAMCAST
  return (int32_t)log2(bufIdByScript);
#else
  return (int32_t)std::log2(bufIdByScript);
#endif
}
inline int32_t GetScriptBufferId(int32_t bufIdBySurf) {
  return (int32_t)std::pow(2, bufIdBySurf);
}

// Meh
inline void LinkBuffers(int linkCode, int currentBufferId,
                        Background2D* currentBuffer);

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto