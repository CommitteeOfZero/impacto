#include "saveicon.h"
#include "../../../profile/hud/saveicon.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

SaveIconAnimation::SaveIconAnimation(
    float activeAnimationDuration, float fadeInDuration, float fadeOutDuration,
    std::span<const Sprite, Profile::SaveIcon::CHLCC_SAVE_ICON_SPRITES>
        sprites) {
  std::ranges::copy(sprites, Sprites.begin());
  ActiveAnimation.SetDuration(activeAnimationDuration);
  ActiveAnimation.LoopMode = AnimationLoopMode::Loop;

  FadeAnimation.DurationIn = fadeInDuration;
  FadeAnimation.DurationOut = fadeOutDuration;
}
void SaveIconAnimation::Update(float dt) {
  if (FadeAnimation.IsIn() && !ActiveAnimation.IsPlaying()) {
    ActiveAnimation.StartIn(true);
  }

  FadeAnimation.Update(dt);
  ActiveAnimation.Update(dt);
}

void SaveIconAnimation::Render(glm::vec2 position) const {
  if (FadeAnimation.IsOut()) return;

  glm::vec4 col(1.0f);
  col.a = FadeAnimation.Progress;
  Renderer->DrawSprite(CurrentSprite(), position, col);
}

Sprite SaveIconAnimation::CurrentSprite() const {
  if (FadeAnimation.IsIn()) {
    // alter between two sprites
    return ActiveAnimation.Progress < 0.5 ? Sprites[0] : Sprites[1];
  }
  return Sprites[2];
}

void SaveIconAnimation::Show() {
  FadeAnimation.StartIn();
  ActiveAnimation.Stop();
}

void SaveIconAnimation::Hide() {
  FadeAnimation.StartOut();
  ActiveAnimation.Stop();
}
}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto