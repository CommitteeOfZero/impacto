#include "waiticondisplay.h"

#include "../profile/dialogue.h"
#include "../renderer2d.h"

namespace Impacto {
namespace WaitIconDisplay {

static glm::vec2 Position;

static Animation SimpleAnim;
static SpriteAnimation SpriteAnim;

using namespace Impacto::Profile::Dialogue;

void Init() {
  if (WaitIconCurrentType == +WaitIconType::SpriteAnim) {
    SpriteAnim = WaitIconSpriteAnim.Instantiate();
    SpriteAnim.LoopMode = ALM_Loop;
    SpriteAnim.StartIn();
  } else {
    SimpleAnim.DurationIn = WaitIconAnimationDuration;
    SimpleAnim.LoopMode = ALM_Loop;
    SimpleAnim.StartIn();
  }
}

void Update(float dt) {
  if (WaitIconCurrentType == +WaitIconType::SpriteAnim) {
    SpriteAnim.Update(dt);
  } else {
    SimpleAnim.Update(dt);
  }
}
void Render(glm::vec2 pos, glm::vec4 opacityTint) {
  if (WaitIconCurrentType == +WaitIconType::SpriteAnim) {
    Renderer2D::DrawSprite(
        SpriteAnim.CurrentSprite(),
        glm::vec2(pos.x + WaitIconOffset.x, pos.y + WaitIconOffset.y),
        opacityTint);
  } else if (WaitIconCurrentType == +WaitIconType::RotateZ) {
    // TODO: MO6TW only for now
    glm::vec3 euler(SimpleAnim.Progress * 2.0f * M_PI, 0, 0.6f);
    glm::quat quat;
    eulerZYXToQuat(&euler, &quat);

    glm::vec2 vanishingPoint(
        (pos.x + WaitIconOffset.x) + (WaitIconSprite.ScaledWidth() / 2.0f),
        (pos.y + WaitIconOffset.y) + (WaitIconSprite.ScaledHeight() / 2.0f));

    Renderer2D::DrawSprite3DRotated(
        WaitIconSprite,
        glm::vec2(pos.x + WaitIconOffset.x, pos.y + WaitIconOffset.y), 1.0f,
        vanishingPoint, true, quat, opacityTint);
  } else {
    Renderer2D::DrawSprite(
        WaitIconSprite,
        glm::vec2(pos.x + WaitIconOffset.x, pos.y + WaitIconOffset.y),
        opacityTint, glm::vec2(1.0f), SimpleAnim.Progress * 2.0f * M_PI);
  }
}

}  // namespace WaitIconDisplay
}  // namespace Impacto