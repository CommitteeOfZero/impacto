#pragma once

#include "../../spriteanimation.h"
#include "../../hud/delusiontrigger.h"

namespace Impacto {
namespace CCLCC {

class DelusionTrigger : public Impacto::DelusionTrigger::DelusionTriggerBase {
 public:
  enum DelusionsShown {
    Delusion_Both = 0,
    Delusion_PosOnly = 1,
    Delusion_NegOnly = 2
  };

  DelusionTrigger();

  void Hide() override;
  void Update(float dt) override;
  void Render() override;
  bool Show(int bgOverlayBgBufferId, int circlesBgBufferId,
            int availableDelusions) override;
  bool CheckTransitionAnimationComplete() override;
  bool CheckStartTransitionComplete() override;

 private:
  int MtrgAlphaCt = 0;
  int MtrgAng = 0;
  int LastDelusionState = 0xFF;
  Sprite PositiveDelusionSprite;
  Sprite NegativeDelusionSprite;
  Sprite BgOverlaySprite;

  void RenderPositiveToNeutralTransition(float spinAngle, int spinAlpha);
  void RenderNegativeToNeutralTransition(float spinAngle, int spinAlpha);
  void RenderNeutralToNegativeTransition(float spinAngle, int spinAlpha);
  void RenderNeutralToPositiveTransition(float spinAngle, int spinAlpha);
  void RenderStartTransition(float spinAngle, int spinAlpha);
  void RenderEndNeutralTransition(float spinAngle, int spinAlpha);
  void RenderStable(float spinAngle, int spinAlpha);
};

}  // namespace CCLCC
}  // namespace Impacto