#pragma once

#include "../../spriteanimation.h"
#include "../../ui/menu.h"

namespace Impacto {
namespace CCLCC {

class DelusionTrigger {
 public:
  enum DelusionState { DS_Neutral, DS_Positive, DS_Negative };
  enum DelusionsShown {
    Delusion_Both = 0,
    Delusion_PosOnly = 1,
    Delusion_NegOnly = 2
  };

  DelusionTrigger();

  void Hide();
  void Update(float dt);
  void Render();
  bool Show(int bgOverlayBgBufferId, int circlesBgBufferId,
            int availableDelusions);
  bool CheckTransitionAnimationComplete();
  bool CheckStartTransitionComplete();

  static inline std::unique_ptr<DelusionTrigger> Implementation = nullptr;

 private:
  int MtrgAlphaCt = 0;
  int MtrgAng = 0;
  int& DelusionState;
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