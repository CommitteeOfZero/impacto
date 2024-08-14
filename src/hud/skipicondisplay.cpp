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
  if (SkipIconCurrentType == +SkipIconType::SpriteAnim) {
    SpriteAnim = SkipIconSpriteAnim.Instantiate();
    SpriteAnim.LoopMode = ALM_Stop;
    SpriteAnim.SkipOnSkipMode = false;
    SpriteAnim.StartIn();
  } else if (SkipIconCurrentType == +SkipIconType::SpriteAnimFixed) {
    SpriteAnim = SkipIconSpriteAnim.Instantiate();
    SpriteAnim.LoopMode = ALM_Stop;
    SpriteAnim.SkipOnSkipMode = false;
    FixedSpriteAnim = static_cast<FixedSpriteAnimation&>(SpriteAnim);
    FixedSpriteAnim.Def->FixSpriteId = SkipIconFixedSpriteId;
    FixedSpriteAnim.StartIn();
  }
}

void Update(float dt) {
  if (SkipIconCurrentType == +SkipIconType::SpriteAnim) {
    if ((MesSkipMode & (SkipModeFlags::SkipRead | SkipModeFlags::SkipAll)) &&
        SpriteAnim.Direction == -1)
      SpriteAnim.StartIn();
    if (!(MesSkipMode & (SkipModeFlags::SkipRead | SkipModeFlags::SkipAll)) &&
        SpriteAnim.Direction == 1)
      SpriteAnim.StartOut();

    SpriteAnim.Update(dt);
  } else if (SkipIconCurrentType == +SkipIconType::SpriteAnimFixed) {
    if ((MesSkipMode & (SkipModeFlags::SkipRead | SkipModeFlags::SkipAll)) &&
        FixedSpriteAnim.Direction == -1)
      FixedSpriteAnim.StartIn();
    if (!(MesSkipMode & (SkipModeFlags::SkipRead | SkipModeFlags::SkipAll)) &&
        FixedSpriteAnim.Direction == 1)
      FixedSpriteAnim.StartOut();

    FixedSpriteAnim.Update(dt);
  } else if (SkipIconCurrentType == +SkipIconType::CHLCC) {
    Progress += dt * SkipIconRotationSpeed;
    if (Progress > 1.0f) Progress = 0.0f;
  }
}

void Render(glm::vec4 opacityTint) {
  if (SkipIconCurrentType == +SkipIconType::SpriteAnim) {
    if (!SpriteAnim.IsOut()) {
      glm::vec4 col = glm::vec4(1.0f, 1.0f, 1.0f, opacityTint.a);
      Renderer->DrawSprite(SpriteAnim.CurrentSprite(),
                           glm::vec2(SkipIconOffset.x, SkipIconOffset.y), col);
    }
  } else if (SkipIconCurrentType == +SkipIconType::SpriteAnimFixed) {
    if (!FixedSpriteAnim.IsOut() &&
        !(FixedSpriteAnim.Direction == -1 &&
          FixedSpriteAnim.Progress ==
              FixedSpriteAnim.GetFixedSpriteProgress())) {
      Renderer->DrawSprite(FixedSpriteAnim.CurrentSprite(),
                           glm::vec2(SkipIconOffset.x, SkipIconOffset.y),
                           opacityTint);
    }
  } else if (SkipIconCurrentType == +SkipIconType::CHLCC) {
    if (MesSkipMode & (SkipModeFlags::SkipRead | SkipModeFlags::SkipAll)) {
      Renderer->DrawSprite(
          AutoSkipArrowsSprite, glm::vec2(SkipIconOffset.x, SkipIconOffset.y),
          opacityTint, glm::vec2(1.0f), Progress * 2 * (float)M_PI);
      Renderer->DrawSprite(SkipIconSprite,
                           glm::vec2(SkipIconOffset.x, SkipIconOffset.y),
                           opacityTint);
    }
  }
}

}  // namespace SkipIconDisplay
}  // namespace Impacto