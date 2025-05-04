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
      SpriteAnim.LoopMode = AnimationLoopMode::Loop;
      SpriteAnim.StartIn();
      break;

    case WaitIconType::SpriteAnimFixed:
      SpriteAnim = WaitIconSpriteAnim.Instantiate();
      SpriteAnim.LoopMode = AnimationLoopMode::Stop;
      FixedSpriteAnim = static_cast<FixedSpriteAnimation&>(SpriteAnim);
      FixedSpriteAnim.Def->FixSpriteId = WaitIconFixedSpriteId;
      break;

    default:
      SimpleAnim.DurationIn = WaitIconAnimationDuration;
      SimpleAnim.LoopMode = AnimationLoopMode::Loop;
      SimpleAnim.StartIn();
      break;
  }
}

void Update(float dt) {
  switch (WaitIconCurrentType) {
    case WaitIconType::None:
      return;

    case WaitIconType::SpriteAnim: {
      bool showWaitIcon = GetFlag(Profile::ScriptVars::SF_SHOWWAITICON);
      if (showWaitIcon && SpriteAnim.IsOut())
        SpriteAnim.StartIn();
      else if (!showWaitIcon && SpriteAnim.IsIn())
        SpriteAnim.StartOut();

      SpriteAnim.Update(dt);
      break;
    }

    case WaitIconType::SpriteAnimFixed: {
      bool showWaitIcon = GetFlag(Profile::ScriptVars::SF_SHOWWAITICON);
      if (showWaitIcon && FixedSpriteAnim.IsOut())
        FixedSpriteAnim.StartIn();
      else if (!showWaitIcon && FixedSpriteAnim.IsIn())
        FixedSpriteAnim.StartOut();

      FixedSpriteAnim.Update(dt);
      break;
    }

    default:
      SimpleAnim.Update(dt);
      break;
  }
}

static void RenderSpriteAnim(glm::vec2 pos, glm::vec4 opacityTint,
                             DialoguePageMode mode) {
  if (!GetFlag(Profile::ScriptVars::SF_SHOWWAITICON)) return;

  glm::vec2 offset = WaitIconOffset;

  if (DialogueBoxCurrentType == +DialogueBoxType::CHLCC) {
    // To deal with multiple DialogueBox
    opacityTint = glm::vec4(1.0f, 1.0f, 1.0f, opacityTint.a);

    // Erin DialogueBox
    if (mode == DPM_REV)
      offset = Impacto::Profile::CHLCC::DialogueBox::REVWaitIconOffset;
  }

  Renderer->DrawSprite(SpriteAnim.CurrentSprite(), pos + offset, opacityTint);
}

static void RenderSpriteAnimFixed(glm::vec4 opacityTint) {
  if (FixedSpriteAnim.Progress == 0.0f) return;

  Renderer->DrawSprite(FixedSpriteAnim.CurrentSprite(),
                       glm::vec2(WaitIconOffset.x, WaitIconOffset.y),
                       opacityTint);
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

    default: {
      if (!GetFlag(Profile::ScriptVars::SF_SHOWWAITICON)) return;

      const CornersQuad dest =
          WaitIconSprite.ScaledBounds()
              .Rotate(SimpleAnim.Progress * 2.0f * (float)M_PI,
                      WaitIconSprite.Center())
              .Translate(pos + WaitIconOffset);
      Renderer->DrawSprite(WaitIconSprite, dest, opacityTint);

      return;
    }
  }
}

}  // namespace WaitIconDisplay
}  // namespace Impacto