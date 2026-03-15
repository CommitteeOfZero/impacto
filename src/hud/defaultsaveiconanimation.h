#pragma once

#include "../animation.h"
#include "../spriteanimation.h"

namespace Impacto {
namespace UI {

class DefaultSaveIconAnimation {
 public:
  SpriteAnimation ForegroundAnimation;
  Animation FadeAnimation;
  Sprite BackgroundSprite;
  glm::vec2 BackgroundOffset;
  float BackgroundMaxAlpha;
  DefaultSaveIconAnimation() = default;
  DefaultSaveIconAnimation(SpriteAnimationDef& foregroundAnimDef,
                           Sprite backgroundSprite, glm::vec2 backgroundOffset,
                           float backgroundMaxAlpha, float fadeInDuration,
                           float fadeOutDuration);

  void Update(float dt);
  void Render(glm::vec2 position);
  Sprite GetBackgroundSprite() const { return BackgroundSprite; }
  glm::vec2 GetBackgroundOffset() const { return BackgroundOffset; }
  float GetBackgroundMaxAlpha() const { return BackgroundMaxAlpha; }

  void Show();
  void Hide();
};

}  // namespace UI
}  // namespace Impacto
