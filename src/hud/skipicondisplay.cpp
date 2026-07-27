#include "skipicondisplay.h"

#include "../profile/dialogue.h"
#include "../renderer/renderer.h"
#include "../text/dialoguepage.h"

namespace Impacto {
namespace SkipIconDisplay {

static std::optional<SpriteAnimation> SpriteAnim;
static std::optional<FixedSpriteAnimation> FixedSpriteAnim;
static Animation SimpleAnimation;

using namespace Impacto::Profile::Dialogue;

void Init() {
  switch (SkipIconCurrentType) {
    case SkipIconType::SpriteAnim:
      SpriteAnim.emplace(SkipIconSpriteAnimationDef->Instantiate());
      SpriteAnim->LoopMode = AnimationLoopMode::Stop;
      SpriteAnim->StartIn();
      break;

    case SkipIconType::SpriteAnimFixed:
      FixedSpriteAnim.emplace(SkipIconFixedSpriteAnimationDef->Instantiate());
      FixedSpriteAnim->LoopMode = AnimationLoopMode::Stop;
      FixedSpriteAnim->StartIn();
      break;

    case SkipIconType::CHLCC:
      SimpleAnimation.SetDuration(SkipIconRotationDuration);
      SimpleAnimation.LoopMode = AnimationLoopMode::Loop;
      SimpleAnimation.StartIn();
      break;

    case SkipIconType::None:
    case SkipIconType::Fixed:
      break;
  }
}

void Update(float dt) {
  switch (SkipIconCurrentType) {
    case SkipIconType::SpriteAnim:
      if (SkipModeEnabled && SpriteAnim->Direction == AnimationDirection::Out)
        SpriteAnim->StartIn();
      if (!SkipModeEnabled && SpriteAnim->Direction == AnimationDirection::In)
        SpriteAnim->StartOut();

      SpriteAnim->Update(dt);
      break;

    case SkipIconType::SpriteAnimFixed:
      if (SkipModeEnabled &&
          FixedSpriteAnim->Direction == AnimationDirection::Out)
        FixedSpriteAnim->StartIn();
      if (!SkipModeEnabled &&
          FixedSpriteAnim->Direction == AnimationDirection::In)
        FixedSpriteAnim->StartOut();

      FixedSpriteAnim->Update(dt);
      break;

    case SkipIconType::CHLCC:
      SimpleAnimation.Update(dt);
      break;

    case SkipIconType::None:
    case SkipIconType::Fixed:
      break;
  }
}

void Render(glm::vec4 opacityTint) {
  switch (SkipIconCurrentType) {
    case SkipIconType::SpriteAnim: {
      if (!SpriteAnim->IsOut()) {
        glm::vec4 col = glm::vec4(1.0f, 1.0f, 1.0f, opacityTint.a);
        Renderer->DrawSprite(SpriteAnim->CurrentSprite(),
                             glm::vec2(SkipIconOffset.x, SkipIconOffset.y),
                             col);
      }
      break;
    }

    case SkipIconType::SpriteAnimFixed:
      if (!FixedSpriteAnim->IsOut() &&
          !(FixedSpriteAnim->Direction == AnimationDirection::Out &&
            FixedSpriteAnim->Progress ==
                FixedSpriteAnim->GetFixedSpriteProgress())) {
        Renderer->DrawSprite(FixedSpriteAnim->CurrentSprite(),
                             glm::vec2(SkipIconOffset.x, SkipIconOffset.y),
                             opacityTint);
      }
      break;

    case SkipIconType::CHLCC:
      if (SkipModeEnabled) {
        const CornersQuad arrowsDest =
            AutoSkipArrowsSprite.ScaledBounds()
                .RotateAroundCenter(SimpleAnimation.Progress * 2.0f *
                                    std::numbers::pi_v<float>)
                .Translate(SkipIconOffset);
        Renderer->DrawSprite(AutoSkipArrowsSprite, arrowsDest, opacityTint);

        Renderer->DrawSprite(SkipIconSprite, SkipIconOffset, opacityTint);
      }
      break;

    case SkipIconType::None:
    case SkipIconType::Fixed:
      break;
  }
}

}  // namespace SkipIconDisplay
}  // namespace Impacto