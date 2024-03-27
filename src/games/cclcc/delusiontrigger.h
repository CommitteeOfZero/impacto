#pragma once

#include "../../spriteanimation.h"
#include "../../hud/delusiontrigger.h"

namespace Impacto {
namespace CCLCC {

class DelusionTrigger : public Impacto::DelusionTrigger::DelusionTriggerBase {
 public:
  DelusionTrigger();

  void Hide();
  void Update(float dt);
  void Render();
  bool Show(int param_1, int param_2, int param_3);

 private:
  int MtrgAlphaCt;
  int MtrgAng;
};

}  // namespace CCLCC
}  // namespace Impacto