#include "skipicondisplay.h"

#include "../profile/dialogue.h"
#include "../renderer/renderer.h"

namespace Impacto {
namespace SkipIconDisplay {

static SpriteAnimation SpriteAnim;
static FixedSpriteAnimation FixedSpriteAnim;
static float Progress = 0.0f;

using namespace Impacto::Profile::Dialogue;

void Init() {
  switch (SkipIconCurrentType) {
    case SkipIconType::SpriteAnim:
      SpriteAnim = SkipIconSpriteAnim.Instantiate();
      SpriteAnim.LoopMode = AnimationLoopMode::Stop;
      SpriteAnim.StartIn();
      break;

    case SkipIconType::SpriteAnimFixed:
      SpriteAnim = SkipIconSpriteAnim.Instantiate();
      SpriteAnim.LoopMode = AnimationLoopMode::Stop;
      FixedSpriteAnim = static_cast<FixedSpriteAnimation&>(SpriteAnim);
      FixedSpriteAnim.Def->FixSpriteId = SkipIconFixedSpriteId;
      FixedSpriteAnim.StartIn();
      break;

    case SkipIconType::None:
    case SkipIconType::Fixed:
    case SkipIconType::CHLCC:
      break;
  }
}

void Update(float dt) {
  switch (SkipIconCurrentType) {
    case SkipIconType::SpriteAnim:
      if (SkipModeEnabled && SpriteAnim.Direction == AnimationDirection::Out)
        SpriteAnim.StartIn();
      if (!SkipModeEnabled && SpriteAnim.Direction == AnimationDirection::In)
        SpriteAnim.StartOut();

      SpriteAnim.Update(dt);
      break;

    case SkipIconType::SpriteAnimFixed:
      if (SkipModeEnabled &&
          FixedSpriteAnim.Direction == AnimationDirection::Out)
        FixedSpriteAnim.StartIn();
      if (!SkipModeEnabled &&
          FixedSpriteAnim.Direction == AnimationDirection::In)
        FixedSpriteAnim.StartOut();

      FixedSpriteAnim.Update(dt);
      break;

    case SkipIconType::CHLCC:
      Progress += dt * SkipIconRotationSpeed;
      Progress -= (int)Progress;  // Progress %= 1.0f
      break;

    case SkipIconType::None:
    case SkipIconType::Fixed:
      break;
  }
}

void Render(glm::vec4 opacityTint) {
  switch (SkipIconCurrentType) {
    case SkipIconType::SpriteAnim: {
      if (!SpriteAnim.IsOut()) {
        glm::vec4 col = glm::vec4(1.0f, 1.0f, 1.0f, opacityTint.a);
        Renderer->DrawSprite(SpriteAnim.CurrentSprite(),
                             glm::vec2(SkipIconOffset.x, SkipIconOffset.y),
                             col);
      }
      break;
    }

    case SkipIconType::SpriteAnimFixed:
      if (!FixedSpriteAnim.IsOut() &&
          !(FixedSpriteAnim.Direction == AnimationDirection::Out &&
            FixedSpriteAnim.Progress ==
                FixedSpriteAnim.GetFixedSpriteProgress())) {
        Renderer->DrawSprite(FixedSpriteAnim.CurrentSprite(),
                             glm::vec2(SkipIconOffset.x, SkipIconOffset.y),
                             opacityTint);
      }
      break;

    case SkipIconType::CHLCC:
      if (SkipModeEnabled) {
        const CornersQuad arrowsDest =
            AutoSkipArrowsSprite.ScaledBounds()
                .RotateAroundCenter(Progress * 2.0f * std::numbers::pi_v<float>)
                .Translate(SkipIconOffset);
        Renderer->DrawSprite(AutoSkipArrowsSprite, arrowsDest, opacityTint);

        Renderer->DrawSprite(SkipIconSprite,
                             glm::vec2(SkipIconOffset.x, SkipIconOffset.y),
                             opacityTint);
      }
      break;

    case SkipIconType::None:
    case SkipIconType::Fixed:
      break;
  }
}

}  // namespace SkipIconDisplay
}  // namespace Impacto