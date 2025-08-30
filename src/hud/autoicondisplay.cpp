#include "autoicondisplay.h"

#include "../profile/dialogue.h"
#include "../renderer/renderer.h"

namespace Impacto {
namespace AutoIconDisplay {

static SpriteAnimation SpriteAnim;
static FixedSpriteAnimation FixedSpriteAnim;
static float Progress = 0.0f;

using namespace Impacto::Profile::Dialogue;

void Init() {
  switch (AutoIconCurrentType) {
    case AutoIconType::SpriteAnim:
      SpriteAnim = AutoIconSpriteAnim.Instantiate();
      SpriteAnim.LoopMode = AnimationLoopMode::Stop;
      SpriteAnim.SkipOnSkipMode = false;
      SpriteAnim.StartIn();
      break;

    case AutoIconType::SpriteAnimFixed:
      SpriteAnim = AutoIconSpriteAnim.Instantiate();
      SpriteAnim.LoopMode = AnimationLoopMode::Stop;
      SpriteAnim.SkipOnSkipMode = false;
      FixedSpriteAnim = static_cast<FixedSpriteAnimation&>(SpriteAnim);
      FixedSpriteAnim.Def->FixSpriteId = AutoIconFixedSpriteId;
      FixedSpriteAnim.StartIn();
      break;

    case AutoIconType::None:
    case AutoIconType::Fixed:
    case AutoIconType::CHLCC:
      break;
  }
}

void Update(float dt) {
  switch (AutoIconCurrentType) {
    case AutoIconType::SpriteAnim:
      if ((MesSkipMode & SkipModeFlags::Auto) &&
          SpriteAnim.Direction == AnimationDirection::Out)
        SpriteAnim.StartIn();
      if (!(MesSkipMode & SkipModeFlags::Auto) &&
          SpriteAnim.Direction == AnimationDirection::In)
        SpriteAnim.StartOut();

      SpriteAnim.Update(dt);
      break;

    case AutoIconType::SpriteAnimFixed:
      if ((MesSkipMode & SkipModeFlags::Auto) &&
          FixedSpriteAnim.Direction == AnimationDirection::Out)
        FixedSpriteAnim.StartIn();
      if (!(MesSkipMode & SkipModeFlags::Auto) &&
          FixedSpriteAnim.Direction == AnimationDirection::In)
        FixedSpriteAnim.StartOut();

      FixedSpriteAnim.Update(dt);
      break;

    case AutoIconType::CHLCC:
      Progress += dt * AutoIconRotationSpeed;
      Progress -= (int)Progress;  // Progress %= 1.0f
      break;

    case AutoIconType::None:
    case AutoIconType::Fixed:
      break;
  }
}

void Render(glm::vec4 opacityTint) {
  switch (AutoIconCurrentType) {
    case AutoIconType::SpriteAnim: {
      if (!SpriteAnim.IsOut()) {
        glm::vec4 col = glm::vec4(1.0f, 1.0f, 1.0f, opacityTint.a);
        Renderer->DrawSprite(SpriteAnim.CurrentSprite(),
                             glm::vec2(AutoIconOffset.x, AutoIconOffset.y),
                             col);
      }
      break;
    }

    case AutoIconType::SpriteAnimFixed:
      if (!FixedSpriteAnim.IsOut() &&
          !(FixedSpriteAnim.Direction == AnimationDirection::Out &&
            FixedSpriteAnim.Progress ==
                FixedSpriteAnim.GetFixedSpriteProgress())) {
        Renderer->DrawSprite(FixedSpriteAnim.CurrentSprite(),
                             glm::vec2(AutoIconOffset.x, AutoIconOffset.y),
                             opacityTint);
      }
      break;

    case AutoIconType::CHLCC:
      if (MesSkipMode & SkipModeFlags::Auto) {
        const CornersQuad arrowsDest =
            AutoSkipArrowsSprite.ScaledBounds()
                .RotateAroundCenter(Progress * 2.0f * std::numbers::pi_v<float>)
                .Translate(AutoIconOffset);
        Renderer->DrawSprite(AutoSkipArrowsSprite, arrowsDest, opacityTint);

        Renderer->DrawSprite(AutoIconSprite, AutoIconOffset, opacityTint);
      }
      break;

    case AutoIconType::None:
    case AutoIconType::Fixed:
      break;
  }
}

}  // namespace AutoIconDisplay
}  // namespace Impacto