#pragma once
#include "../../impacto.h"
#include "../../background2d.h"
#include "../../character2d.h"

namespace Impacto {
namespace Vm {
namespace Interface {

void UpdateBackground2D();

// Because yes
inline int GetBufferId(int bufIdByScript) {
  return (int)std::log2(bufIdByScript);
}
inline int GetScriptBufferId(int bufIdBySurf) {
  return (int)std::pow(2, bufIdBySurf);
}

// Meh
void LinkBuffers(int linkCode, int currentBufferId,
                 Background2D* currentBuffer);

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto