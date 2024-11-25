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
  }
}

void Update(float dt) {
  switch (AutoIconCurrentType) {
    case AutoIconType::SpriteAnim:
      if ((MesSkipMode & SkipModeFlags::Auto) && SpriteAnim.Direction == -1)
        SpriteAnim.StartIn();
      if (!(MesSkipMode & SkipModeFlags::Auto) && SpriteAnim.Direction == 1)
        SpriteAnim.StartOut();

      SpriteAnim.Update(dt);
      break;

    case AutoIconType::SpriteAnimFixed:
      if ((MesSkipMode & SkipModeFlags::Auto) &&
          FixedSpriteAnim.Direction == -1)
        FixedSpriteAnim.StartIn();
      if (!(MesSkipMode & SkipModeFlags::Auto) &&
          FixedSpriteAnim.Direction == 1)
        FixedSpriteAnim.StartOut();

      FixedSpriteAnim.Update(dt);
      break;

    case AutoIconType::CHLCC:
      Progress += dt * AutoIconRotationSpeed;
      Progress -= (int)Progress;  // Progress %= 1.0f
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
          !(FixedSpriteAnim.Direction == -1 &&
            FixedSpriteAnim.Progress ==
                FixedSpriteAnim.GetFixedSpriteProgress())) {
        Renderer->DrawSprite(FixedSpriteAnim.CurrentSprite(),
                             glm::vec2(AutoIconOffset.x, AutoIconOffset.y),
                             opacityTint);
      }
      break;

    case AutoIconType::CHLCC:
      if (MesSkipMode & SkipModeFlags::Auto) {
        Renderer->DrawSprite(
            AutoSkipArrowsSprite, glm::vec2(AutoIconOffset.x, AutoIconOffset.y),
            opacityTint, glm::vec2(1.0f), Progress * 2 * (float)M_PI);
        Renderer->DrawSprite(AutoIconSprite,
                             glm::vec2(AutoIconOffset.x, AutoIconOffset.y),
                             opacityTint);
      }
      break;
  }
}

}  // namespace AutoIconDisplay
}  // namespace Impacto