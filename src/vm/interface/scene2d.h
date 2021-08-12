#pragma once
#include "../../impacto.h"
#include "../../background2d.h"
#include "../../character2d.h"

namespace Impacto {
namespace Vm {
namespace Interface {

enum LinkDirection { LD_Off, LD_Up, LD_Down, LD_Left, LD_Right };

void UpdateBackground2D();

void UpdateCharacter2D();

// Because yes
inline int GetBufferId(int bufIdByScript) { return std::log2(bufIdByScript); }
inline int GetScriptBufferId(int bufIdBySurf) {
  return std::pow(2, bufIdBySurf);
}

// Meh
inline void LinkBuffers(int linkCode, int currentBufferId,
                        Background2D* currentBuffer);

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto