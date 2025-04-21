#pragma once

#include "../../animation.h"
#include "../../sequencedanimation.h"
#include "../../spritesheet.h"
#include <array>

namespace Impacto {
namespace CHLCC {

class IntroSequence {
 public:
  IntroSequence();
  ~IntroSequence();

  void Update(float dt);

  SequencedAnimation IntroAnimation;
  Animation PanningAnimation;
  Animation StarBounceAnimation;
  Animation ExplodingStarAnimation;
  Animation ExplodingStarRotationAnimation;
  Animation FallingStarsAnimation;
  Animation FallingStarsRotationAnimation;
  Animation LogoFadeAnimation;
  Animation LCCLogoAnimation;
  Animation LogoStarHighlightAnimation;
  Animation DelusionADVAnimation;
  Animation DelusionADVHighlightAnimation;
  Animation SeiraAnimation;
  Animation LogoPopOutAnimation;
  Animation CopyrightAnimation;

  void Render();

 private:
  void DrawBackground() const;
  void DrawFallingStars() const;
  void DrawExplodingStars() const;
  void DrawBouncingStar() const;
  void DrawCHLogo() const;
  void DrawLCCLogo() const;
  void DrawDelusionADVText() const;
  void DrawSeira() const;

  std::array<std::pair<glm::vec2, float>, 20> FallingStarSeeds;
  Sprite FallingStarsMask;
};

}  // namespace CHLCC
}  // namespace Impacto