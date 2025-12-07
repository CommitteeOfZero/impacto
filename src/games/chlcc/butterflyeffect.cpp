#include "butterflyeffect.h"
#include "../../profile/ui/gamespecific.h"

#include <numeric>

using namespace Impacto::Profile::GameSpecific;
using namespace Impacto::Profile::ScriptVars;

namespace Impacto {
namespace UI {
namespace CHLCC {

Butterfly::Butterfly() {
  Flap.DurationIn = ButterflyFlapFrameDuration * ButterflyFrameCount;
  Flap.LoopMode = AnimationLoopMode::Loop;
  Init();
}

void Butterfly::Init(bool loop) {
  Size = static_cast<uint16_t>(CALCrnd(34) + 40);
  Position.x = static_cast<float>(CALCrnd(1480) - 100);
  Position.y = static_cast<float>(loop ? (Size + 720) : CALCrnd(820) - 100);
  const float angle = ((CALCrnd(60) - 30)) * std::numbers::pi_v<float> / 180.0f;
  Velocity.x = std::sin(angle) * Size * 0.25f;
  Velocity.y = -std::cos(angle) * Size * 0.25f;

  const int maxFrames =
      static_cast<int>(ButterflyFlapFrameDuration * ButterflyFrameCount * 60);
  const int randomProgress = CALCrnd(maxFrames);
  Flap.Progress = static_cast<float>(randomProgress / maxFrames);
  Flap.StartIn();
}

void Butterfly::Update(float dt) {
  Flap.Update(dt);

  Position += dt * 60.0f * Velocity;
  if (Position.y + Size < 0.0f) {
    Init(true);
  }
}

ButterflyEffect::ButterflyEffect() {
  FadeAnimation.SetDuration(ButterflyFadeDuration);
}

void Butterfly::Render(float alphaMultipiler) {
  const uint8_t butterflyIndex =
      static_cast<uint8_t>(Flap.Progress * ButterflyFrameCount);
  const float sizeF = static_cast<float>(Size);
  const RectF dest{Position.x - sizeF / 2.0f, Position.y - sizeF / 2.0f, sizeF,
                   sizeF};
  const float alpha = (ScrWork[SW_BUTTERFLY_ALPHA] * alphaMultipiler) *
                      (Size / 148.0f + 1 / 2.0f) / 256.0f;
  constexpr static auto butterflyFrameSpriteMap =
      std::to_array({0, 1, 2, 1, 0, 3, 4, 3});
  Renderer->DrawSprite(
      ButterflySprites[butterflyFrameSpriteMap[butterflyIndex]], dest,
      {glm::vec3{1.0f}, alpha});
}

void ButterflyEffect::Init() {
  for (auto& butterfly : Butterflies) {
    butterfly.Init();
  }
}

void ButterflyEffect::Update(float dt) {
  if (!ScrWork[SW_BUTTERFLY_COUNT]) {
    if (FadeAnimation.IsIn()) {
      FadeAnimation.StartOut();
    }
    return;
  }

  if (FadeAnimation.IsOut()) {
    FadeAnimation.StartIn();
  }

  FadeAnimation.Update(dt);
  for (auto& butterfly : Butterflies) {
    butterfly.Update(dt);
  }
}
void ButterflyEffect::Render() {
  for (auto& butterfly : Butterflies) {
    butterfly.Render(FadeAnimation.Progress);
  }
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto