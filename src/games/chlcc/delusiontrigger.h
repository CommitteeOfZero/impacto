#pragma once

#include "../../spriteanimation.h"
#include "../../hud/delusiontrigger.h"

namespace Impacto {
namespace CHLCC {
class DelusionTrigger : public Impacto::DelusionTrigger::DelusionTriggerBase {
 public:
  DelusionTrigger();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

 protected:
  int maskScaleFactor;
  int spinAngle;
  int spinRate;
  int underLayerAlpha;
  int backgroundAlpha;

  int AnimCounter;
  int AnimationState;

  glm::vec4 TriggerOnTint;
  int TriggerOnTintAlpha;

  void UpdateShowing();
  void UpdateShown();
  void UpdateHiding();
  void PlayClickSound();

  int UnderlayerXOffset, UnderlayerXRate;
  int ShakeState;
  int MaskOffsetX;
};

}  // namespace CHLCC
}  // namespace Impacto