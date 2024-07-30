#include "waiticondisplay.h"

#include "../profile/dialogue.h"
#include "../renderer/renderer.h"
#include "../profile/games/chlcc/dialoguebox.h"

namespace Impacto {
namespace WaitIconDisplay {

static Animation SimpleAnim;
static SpriteAnimation SpriteAnim;
static FixedSpriteAnimation FixedSpriteAnim;

using namespace Impacto::Profile::Dialogue;
using namespace Impacto::Profile::CHLCC;

void Init() {
  if (WaitIconCurrentType == +WaitIconType::SpriteAnim) {
    SpriteAnim = WaitIconSpriteAnim.Instantiate();
    SpriteAnim.LoopMode = ALM_Loop;
    SpriteAnim.StartIn();
  } else if (WaitIconCurrentType == +WaitIconType::SpriteAnimFixed) {
    SpriteAnim = WaitIconSpriteAnim.Instantiate();
    SpriteAnim.LoopMode = ALM_Stop;
    FixedSpriteAnim = static_cast<FixedSpriteAnimation&>(SpriteAnim);
    FixedSpriteAnim.Def->FixSpriteId = WaitIconFixedSpriteId;
    FixedSpriteAnim.StartIn();
  } else {
    SimpleAnim.DurationIn = WaitIconAnimationDuration;
    SimpleAnim.LoopMode = ALM_Loop;
    SimpleAnim.StartIn();
  }
}

void Update(float dt) {
  if (WaitIconCurrentType == +WaitIconType::SpriteAnim) {
    SpriteAnim.Update(dt);
  } else if (WaitIconCurrentType == +WaitIconType::SpriteAnimFixed) {
    FixedSpriteAnim.Update(dt);
  } else {
    SimpleAnim.Update(dt);
  }
}
void Render(glm::vec2 pos, glm::vec4 opacityTint, DialoguePageMode mode) {
  if (WaitIconCurrentType == +WaitIconType::SpriteAnim) {
    if (DialogueBoxCurrentType == +DialogueBoxType::CHLCC) {
      // To deal with multiple DialogueBox
      glm::vec4 col = glm::vec4(1.0f, 1.0f, 1.0f, opacityTint.a);
      // Erin DialogueBox
      if (mode == DPM_REV) {
        Renderer->DrawSprite(
            SpriteAnim.CurrentSprite(),
            glm::vec2(
                pos.x +
                    Impacto::Profile::CHLCC::DialogueBox::REVWaitIconOffset.x,
                pos.y +
                    Impacto::Profile::CHLCC::DialogueBox::REVWaitIconOffset.y),
            col);

      } else {
        Renderer->DrawSprite(
            SpriteAnim.CurrentSprite(),
            glm::vec2(pos.x + WaitIconOffset.x, pos.y + WaitIconOffset.y), col);
      }
    } else {
      Renderer->DrawSprite(
          SpriteAnim.CurrentSprite(),
          glm::vec2(pos.x + WaitIconOffset.x, pos.y + WaitIconOffset.y),
          opacityTint);
    }
  } else if (WaitIconCurrentType == +WaitIconType::SpriteAnimFixed) {
    Renderer->DrawSprite(
        FixedSpriteAnim.CurrentSprite(),
        glm::vec2(WaitIconOffset.x - 50, WaitIconOffset.y - 50), opacityTint);
  } else if (WaitIconCurrentType == +WaitIconType::RotateZ) {
    // TODO: MO6TW only for now
    glm::vec3 euler(SimpleAnim.Progress * 2.0f * M_PI, 0, 0.6f);
    glm::quat quat;
    eulerZYXToQuat(&euler, &quat);

    glm::vec2 vanishingPoint(
        (pos.x + WaitIconOffset.x) + (WaitIconSprite.ScaledWidth() / 2.0f),
        (pos.y + WaitIconOffset.y) + (WaitIconSprite.ScaledHeight() / 2.0f));

    Renderer->DrawSprite3DRotated(
        WaitIconSprite,
        glm::vec2(pos.x + WaitIconOffset.x, pos.y + WaitIconOffset.y), 1.0f,
        vanishingPoint, true, quat, opacityTint);
  } else if (WaitIconCurrentType == +WaitIconType::None) {
    Renderer->DrawSprite(
        WaitIconSprite,
        glm::vec2(pos.x + WaitIconOffset.x, pos.y + WaitIconOffset.y),
        opacityTint, glm::vec2(1.0f));
  } else {
    Renderer->DrawSprite(
        WaitIconSprite,
        glm::vec2(pos.x + WaitIconOffset.x, pos.y + WaitIconOffset.y),
        opacityTint, glm::vec2(1.0f), SimpleAnim.Progress * 2.0f * (float)M_PI);
  }
}

}  // namespace WaitIconDisplay
}  // namespace Impacto