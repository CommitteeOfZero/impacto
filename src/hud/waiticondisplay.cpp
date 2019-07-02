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
  } else {
    Renderer2D::DrawSprite(
        WaitIconSprite,
        glm::vec2(pos.x + WaitIconOffset.x, pos.y + WaitIconOffset.y),
        opacityTint, glm::vec2(1.0f), SimpleAnim.Progress * 2.0f * M_PI);
  }
}

}  // namespace WaitIconDisplay
}  // namespace Impacto