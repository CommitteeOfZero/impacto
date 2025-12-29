#include "bubbleseffect.h"
#include "../../profile/ui/gamespecific.h"

#include <numeric>

using namespace Impacto::Profile::GameSpecific;
using namespace Impacto::Profile::ScriptVars;

namespace Impacto {
namespace UI {
namespace CHLCC {

Bubble::Bubble() { Init(); }

void Bubble::Init(bool loop) {
  Size = static_cast<uint16_t>(CALCrnd(64) + 10);
  Position.x = static_cast<float>(CALCrnd(1480) - 100);
  Position.y = static_cast<float>(loop ? (Size + 720) : CALCrnd(820) - 100);
  if (!loop) RandAngle = static_cast<uint16_t>(CALCrnd(16384) << 2);
}

void Bubble::Update(float dt) {
  Position.y += dt * 60.0f * -(Size * 0.25f);
  RandAngle += 2048;

  if (Position.y + Size < 0.0f) {
    Init(true);
  }
}

BubblesEffect::BubblesEffect() {
  FadeAnimation.DurationIn = BubbleFadeDuration;
}

void Bubble::Render(float alphaMultipiler) {
  const float sizeF = static_cast<float>(Size);
  const float sine =
      std::sin(RandAngle / 65536.0f * 2 * std::numbers::pi_v<float>);
  const float xPos = Position.x + sizeF * sine * 0.5f - sizeF / 2.0f;
  const RectF dest{xPos, Position.y - sizeF / 2.0f, sizeF, sizeF};
  const float alpha = (ScrWork[SW_BUBBLES_ALPHA] * alphaMultipiler) / 256.0f;
  if (Size < 70) {
    Renderer->DrawSprite(BubbleSpriteSmall, dest, {glm::vec3{1.0f}, alpha});
  } else {
    Renderer->DrawSprite(BubbleSpriteBig, dest, {glm::vec3{1.0f}, alpha});
  }
}

void BubblesEffect::Init() {
  for (auto& bubble : Bubbles) {
    bubble.Init();
  }
}

void BubblesEffect::Update(float dt) {
  if (!ScrWork[SW_BUBBLES_ALPHA]) return;
  if (!ScrWork[SW_BUBBLES_COUNT]) {
    if (FadeAnimation.Direction != AnimationDirection::Out) {
      FadeAnimation.StartOut();
    }
  }

  if (FadeAnimation.IsOut()) {
    FadeAnimation.StartIn();
  }

  FadeAnimation.Update(dt);
  for (auto& bubble : Bubbles) {
    bubble.Update(dt);
  }
}
void BubblesEffect::Render() {
  for (auto& bubble : Bubbles) {
    bubble.Render(FadeAnimation.Progress);
  }
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto