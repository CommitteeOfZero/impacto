#include "waiticondisplay.h"

#include "../renderer/renderer.h"
#include "../profile/dialogue.h"
#include "../profile/scriptvars.h"
#include "../profile/games/chlcc/dialoguebox.h"

#include "../animation.h"

namespace Impacto {
namespace WaitIconDisplay {

static Animation SimpleAnim;
static SpriteAnimation SpriteAnim;
static FixedSpriteAnimation FixedSpriteAnim;

using namespace Impacto::Profile::Dialogue;
using namespace Impacto::Profile::CHLCC;

void Init() {
  if (WaitIconCurrentType == +WaitIconType::None) return;

  switch (WaitIconCurrentType) {
    case WaitIconType::SpriteAnim:
      SpriteAnim = WaitIconSpriteAnim.Instantiate();
      SpriteAnim.LoopMode = ALM_Loop;
      SpriteAnim.StartIn();
      break;
    case WaitIconType::SpriteAnimFixed:
      SpriteAnim = WaitIconSpriteAnim.Instantiate();
      SpriteAnim.LoopMode = ALM_Stop;
      FixedSpriteAnim = static_cast<FixedSpriteAnimation&>(SpriteAnim);
      FixedSpriteAnim.Def->FixSpriteId = WaitIconFixedSpriteId;
      break;
    default:
      SimpleAnim.DurationIn = WaitIconAnimationDuration;
      SimpleAnim.LoopMode = ALM_Loop;
      SimpleAnim.StartIn();
      break;
  }
}

void Update(float dt) {
  switch (WaitIconCurrentType) {
    case WaitIconType::None:
      return;

    case WaitIconType::SpriteAnim:
      SpriteAnim.Update(dt);
      break;

    case WaitIconType::SpriteAnimFixed:
      FixedSpriteAnim.Update(dt);
      break;

    default:
      SimpleAnim.Update(dt);
      break;
  }
}

static void RenderSpriteAnim(glm::vec2 pos, glm::vec4 opacityTint,
                             DialoguePageMode mode) {
  if (GetFlag(Profile::ScriptVars::SF_SHOWWAITICON))
    SpriteAnim.StartIn();
  else
    SpriteAnim.StartOut();

  if (FixedSpriteAnim.IsOut()) return;

  if (DialogueBoxCurrentType == +DialogueBoxType::CHLCC) {
    // To deal with multiple DialogueBox
    glm::vec4 col = glm::vec4(1.0f, 1.0f, 1.0f, opacityTint.a);
    // Erin DialogueBox
    if (mode == DPM_REV) {
      Renderer->DrawSprite(
          SpriteAnim.CurrentSprite(),
          glm::vec2(
              pos.x + Impacto::Profile::CHLCC::DialogueBox::REVWaitIconOffset.x,
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
  return;
}

static void RenderSpriteAnimFixed(glm::vec4 opacityTint) {
  if (GetFlag(Profile::ScriptVars::SF_SHOWWAITICON))
    FixedSpriteAnim.StartIn();
  else
    FixedSpriteAnim.StartOut();

  if (FixedSpriteAnim.IsOut()) return;

  Renderer->DrawSprite(FixedSpriteAnim.CurrentSprite(),
                       glm::vec2(WaitIconOffset.x - 50, WaitIconOffset.y - 50),
                       opacityTint);
  return;
}

static void RenderRotateZ(glm::vec2 pos, glm::vec4 opacityTint) {
  if (!GetFlag(Profile::ScriptVars::SF_SHOWWAITICON)) return;

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
  return;
}

static void RenderFixed(glm::vec4 opacityTint) {
  if (!GetFlag(Profile::ScriptVars::SF_SHOWWAITICON)) return;

  Renderer->DrawSprite(WaitIconSprite, WaitIconOffset, opacityTint);
}

void Render(glm::vec2 pos, glm::vec4 opacityTint, DialoguePageMode mode) {
  switch (WaitIconCurrentType) {
    case WaitIconType::None:
      return;
    case WaitIconType::SpriteAnim:
      RenderSpriteAnim(pos, opacityTint, mode);
      return;
    case WaitIconType::SpriteAnimFixed:
      RenderSpriteAnimFixed(opacityTint);
      return;
    case WaitIconType::RotateZ:
      RenderRotateZ(pos, opacityTint);
      return;
    case WaitIconType::Fixed:
      RenderFixed(opacityTint);
      return;
    default:
      if (!GetFlag(Profile::ScriptVars::SF_SHOWWAITICON)) return;

      Renderer->DrawSprite(
          WaitIconSprite,
          glm::vec2(pos.x + WaitIconOffset.x, pos.y + WaitIconOffset.y),
          opacityTint, glm::vec2(1.0f),
          SimpleAnim.Progress * 2.0f * (float)M_PI);
      return;
  }
}

}  // namespace WaitIconDisplay
}  // namespace Impacto