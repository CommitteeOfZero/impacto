#pragma once

#include "../../spriteanimation.h"
#include "../../hud/delusiontrigger.h"

namespace Impacto {
namespace CCLCC {

class DelusionTrigger : public Impacto::DelusionTrigger::DelusionTriggerBase {
 public:
  DelusionTrigger();

  void Hide() override;
  void Update(float dt) override;
  void Render() override;
  bool Show(int param_1, int param_2, int param_3) override;
  bool CheckTransitionAnimationComplete() override;
  bool CheckStartTransitionComplete() override;

 private:
  int MtrgAlphaCt = 0;
  int MtrgAng = 0;
  int LastDelusionState = 0xFF;

  void RenderPositiveToNeutralTransition(Sprite& bgSprite, float spinAngle,
                                         int spinAlpha);
  void RenderNegativeToNeutralTransition(Sprite& bgSprite, float spinAngle,
                                         int spinAlpha);
  void RenderNeutralToNegativeTransition(Sprite& bgSprite, float spinAngle,
                                         int spinAlpha);
  void RenderNeutralToPositiveTransition(Sprite& bgSprite, float spinAngle,
                                         int spinAlpha);
  void RenderStartTransition(Sprite& bgSprite, float spinAngle, int spinAlpha);
  void RenderEndNeutralTransition(Sprite& bgSprite, float spinAngle,
                                  int spinAlpha);
  void RenderStable(Sprite& bgSprite, float spinAngle, int spinAlpha);
};

}  // namespace CCLCC
}  // namespace Impacto