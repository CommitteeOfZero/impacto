#include "skipicondisplay.h"

#include "../profile/dialogue.h"
#include "../renderer/renderer.h"

namespace Impacto {
namespace SkipIconDisplay {

static SpriteAnimation SpriteAnim;
static FixedSpriteAnimation FixedSpriteAnim;

using namespace Impacto::Profile::Dialogue;

void Init() {
  if (SkipIconCurrentType == +SkipIconType::SpriteAnim) {
    SpriteAnim = SkipIconSpriteAnim.Instantiate();
    SpriteAnim.LoopMode = ALM_Stop;
    SpriteAnim.StartIn();
  } else if (SkipIconCurrentType == +SkipIconType::SpriteAnimFixed) {
    SpriteAnim = SkipIconSpriteAnim.Instantiate();
    SpriteAnim.LoopMode = ALM_Stop;
    FixedSpriteAnim = static_cast<FixedSpriteAnimation&>(SpriteAnim);
    FixedSpriteAnim.Def->FixSpriteId = SkipIconFixedSpriteId;
    FixedSpriteAnim.StartIn();
  }
}

void Update(float dt) {
  if (SkipIconCurrentType == +SkipIconType::SpriteAnim) {
    if ((MesSkipMode & (SkipModeFlags::SkipRead | SkipModeFlags::SkipAll)) && SpriteAnim.Direction == -1)
      SpriteAnim.StartIn();
    if (!(MesSkipMode & (SkipModeFlags::SkipRead | SkipModeFlags::SkipAll)) && SpriteAnim.Direction == 1)
      SpriteAnim.StartOut();

    SpriteAnim.Update(dt);
  } else if (SkipIconCurrentType == +SkipIconType::SpriteAnimFixed) {
    if ((MesSkipMode & (SkipModeFlags::SkipRead | SkipModeFlags::SkipAll)) && FixedSpriteAnim.Direction == -1)
      FixedSpriteAnim.StartIn();
    if (!(MesSkipMode & (SkipModeFlags::SkipRead | SkipModeFlags::SkipAll)) && FixedSpriteAnim.Direction == 1)
      FixedSpriteAnim.StartOut();

    FixedSpriteAnim.Update(dt);
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
    if (!FixedSpriteAnim.IsOut() && !(FixedSpriteAnim.Direction == -1 &&
        FixedSpriteAnim.Progress == FixedSpriteAnim.GetFixedSpriteProgress())) {
      Renderer->DrawSprite(
          FixedSpriteAnim.CurrentSprite(),
          glm::vec2(SkipIconOffset.x - 50, SkipIconOffset.y - 50), opacityTint);
    }
  } else if (SkipIconCurrentType == +SkipIconType::None) {
    if (MesSkipMode & (SkipModeFlags::SkipRead | SkipModeFlags::SkipAll))
      Renderer->DrawSprite(SkipIconSprite,
                           glm::vec2(SkipIconOffset.x, SkipIconOffset.y),
                           opacityTint, glm::vec2(1.0f));
  }
}

}  // namespace SkipIconDisplay
}  // namespace Impacto