#pragma once

#include "../../spriteanimation.h"
#include "../../ui/menu.h"

namespace Impacto {
namespace UI {
namespace CHLCC {
class DelusionTrigger {
 public:
  enum DelusionState { DS_Neutral, DS_Positive, DS_Negative };
  DelusionTrigger();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  static DelusionTrigger& GetInstance() {
    static DelusionTrigger impl;
    return impl;
  };

 protected:
  void UpdateShowing();
  void UpdateShown();
  void UpdateHiding();
  void PlayClickSound();

  int MaskScaleFactor;
  int SpinAngle;
  int SpinRate;
  int UnderlayerAlpha;
  int BackgroundAlpha;

  int AnimCounter;
  int AnimationState;

  glm::vec4 TriggerOnTint;
  int TriggerOnTintAlpha;

  int UnderlayerXOffset, UnderlayerXRate;
  int ShakeState;
  int MaskOffsetX;
  int& DelusionState;
  UI::MenuState State = UI::Hidden;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto