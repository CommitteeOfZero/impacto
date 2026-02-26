#include "waiticondisplay.h"

#include "../renderer/renderer.h"
#include "../profile/dialogue.h"
#include "../profile/scriptvars.h"
#include "../profile/games/chlcc/dialoguebox.h"
#include "../text/dialoguepage.h"

#include "../animation.h"

#include <ranges>
#include <numeric>

namespace Impacto {
namespace WaitIconDisplay {

static Animation SimpleAnim;
static SpriteAnimation SpriteAnim;
static FixedSpriteAnimation FixedSpriteAnim;

using namespace Impacto::Profile::Dialogue;
using namespace Impacto::Profile::ScriptVars;
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

static bool AnyPageWaiting() {
  return std::ranges::any_of(std::views::iota(0, std::ssize(DialoguePages)),
                             [](int dialoguePageId) {
                               return GetFlag(SF_SHOWWAITICON + dialoguePageId);
                             });
}

void Update(float dt) {
  switch (WaitIconCurrentType) {
    case WaitIconType::None:
      return;

    case WaitIconType::SpriteAnim: {
      const bool showWaitIcon = AnyPageWaiting();
      if (showWaitIcon && SpriteAnim.IsOut())
        SpriteAnim.StartIn();
      else if (!showWaitIcon && SpriteAnim.IsIn())
        SpriteAnim.StartOut();

      SpriteAnim.Update(dt);
      break;
    }

    case WaitIconType::SpriteAnimFixed: {
      const bool showWaitIcon = AnyPageWaiting();
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
                             DialoguePageMode mode, int dialoguePageId) {
  if (!GetFlag(SF_SHOWWAITICON + dialoguePageId)) return;

  glm::vec2 offset = WaitIconOffset;

  if (DialogueBoxCurrentType == +DialogueBoxType::CHLCC) {
    // To deal with multiple DialogueBox
    opacityTint = glm::vec4(1.0f, 1.0f, 1.0f, opacityTint.a);

    // Erin DialogueBox
    if (mode == DPM_REV && ScrWork[SW_MESWIN0TYPE] == 1)
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

static void RenderRotateZ(glm::vec2 pos, glm::vec4 opacityTint,
                          int dialoguePageId) {
  if (!GetFlag(SF_SHOWWAITICON + dialoguePageId)) return;

  // TODO: MO6TW only for now
  glm::vec3 euler(SimpleAnim.Progress * 2.0f * std::numbers::pi_v<float>, 0,
                  0.6f);
  glm::quat quat;
  eulerZYXToQuat(&euler, &quat);

  glm::vec2 vanishingPoint = pos + WaitIconOffset + WaitIconSprite.Center();

  CornersQuad dest =
      WaitIconSprite.ScaledBounds().Translate(pos + WaitIconOffset);
  dest.Rotate(quat, {dest.Center(), 0.0f}, 1.0f, vanishingPoint, true);
  Renderer->DrawSprite(WaitIconSprite, dest, opacityTint);
}

static void RenderFixed(glm::vec4 opacityTint, int dialoguePageId) {
  if (!GetFlag(SF_SHOWWAITICON + dialoguePageId)) return;

  Renderer->DrawSprite(WaitIconSprite, WaitIconOffset, opacityTint);
}

void Render(glm::vec2 pos, glm::vec4 opacityTint, DialoguePageMode mode,
            int dialoguePageId) {
  switch (WaitIconCurrentType) {
    case WaitIconType::None:
      return;

    case WaitIconType::SpriteAnim:
      RenderSpriteAnim(pos, opacityTint, mode, dialoguePageId);
      return;

    case WaitIconType::SpriteAnimFixed:
      RenderSpriteAnimFixed(opacityTint);
      return;

    case WaitIconType::RotateZ:
      RenderRotateZ(pos, opacityTint, dialoguePageId);
      return;

    case WaitIconType::Fixed:
      RenderFixed(opacityTint, dialoguePageId);
      return;

    default: {
      if (!GetFlag(SF_SHOWWAITICON + dialoguePageId)) return;

      const CornersQuad dest =
          WaitIconSprite.ScaledBounds()
              .RotateAroundCenter(SimpleAnim.Progress * 2.0f *
                                  std::numbers::pi_v<float>)
              .Translate(pos + WaitIconOffset);
      Renderer->DrawSprite(WaitIconSprite, dest, opacityTint);

      return;
    }
  }
}

}  // namespace WaitIconDisplay
}  // namespace Impacto