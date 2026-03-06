#pragma once

#include "../../../animation.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class SaveIconAnimation {
 public:
  Animation ActiveAnimation;
  Animation FadeAnimation;
  std::array<Sprite, 3> Sprites = {};

  SaveIconAnimation(float activeAnimationDuration, float fadeInDuration,
                    float fadeOutDuration, std::span<const Sprite> sprites);
  void Update(float dt);
  void Render(glm::vec2 position);
  Sprite CurrentSprite() const;

  void Show();
  void Hide();
};
}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto