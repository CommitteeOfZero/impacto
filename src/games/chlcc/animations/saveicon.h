#pragma once

#include "../../../animation.h"
#include "../../../spritesheet.h"
#include "../../../profile/hud/saveicon.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class SaveIconAnimation {
  Animation ActiveAnimation;
  Animation FadeAnimation;
  std::array<Sprite, Profile::SaveIcon::CHLCC_SAVE_ICON_SPRITES> Sprites = {};

 public:
  SaveIconAnimation(
      float activeAnimationDuration, float fadeInDuration,
      float fadeOutDuration,
      std::span<const Sprite, Profile::SaveIcon::CHLCC_SAVE_ICON_SPRITES>
          sprites);
  void Update(float dt);
  void Render(glm::vec2 position) const;
  Sprite CurrentSprite() const;

  void Show();
  void Hide();
};
}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto