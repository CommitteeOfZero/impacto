#include "defaultsaveiconanimation.h"
#include "../renderer/renderer.h"

#include "../profile/hud/saveicon.h"

namespace Impacto {
namespace UI {

DefaultSaveIconAnimation::DefaultSaveIconAnimation(
    SpriteAnimationDef& foregroundAnimDef, Sprite backgroundSprite,
    glm::vec2 backgroundOffset, float backgroundMaxAlpha, float fadeInDuration,
    float fadeOutDuration)
    : BackgroundSprite(backgroundSprite),
      BackgroundOffset(backgroundOffset),
      BackgroundMaxAlpha(backgroundMaxAlpha) {
  ForegroundAnimation = foregroundAnimDef.Instantiate();
  ForegroundAnimation.LoopMode = AnimationLoopMode::Loop;

  FadeAnimation.DurationIn = fadeInDuration;
  FadeAnimation.DurationOut = fadeOutDuration;
}

void DefaultSaveIconAnimation::Update(float dt) {
  FadeAnimation.Update(dt);
  // Images is fading-in and then ForegroundAnimation is playing, and in reverse
  // for fade out
  if (FadeAnimation.IsStopped()) {
    ForegroundAnimation.Update(dt);
  }
}

void DefaultSaveIconAnimation::Render(glm::vec2 position) {
  if (FadeAnimation.IsOut()) return;

  glm::vec4 col(1.0f);
  col.a = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);

  const Sprite bgSprite = GetBackgroundSprite();
  const glm::vec2 bgSize = bgSprite.Bounds.GetSize();
  // run less logic if bg sprite is a stub sprite with {0,0} dimensions
  if (bgSize.x > 0 && bgSize.y > 0) {
    glm::vec4 tint(col);
    tint.a *= GetBackgroundMaxAlpha();
    Renderer->DrawSprite(bgSprite, position + GetBackgroundOffset(), tint);
  }

  // if FadeAnimation is playing, should show full visible sprite
  Renderer->DrawSprite(
      FadeAnimation.IsPlaying()
          ? ForegroundAnimation.Def
                ->Frames[Profile::SaveIcon::FullyVisibleSpriteIndex]
          : ForegroundAnimation.CurrentSprite(),
      position, col);
}

void DefaultSaveIconAnimation::Show() {
  FadeAnimation.StartIn(true);
  ForegroundAnimation.StartIn(true);
}

void DefaultSaveIconAnimation::Hide() {
  FadeAnimation.StartOut();
  ForegroundAnimation.StartOut();
}

}  // namespace UI
}  // namespace Impacto
