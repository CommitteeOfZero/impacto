#include "waiticondisplay.h"

#include "../renderer/renderer.h"
#include "../profile/dialogue.h"
#include "../profile/scriptvars.h"
#include "../text/dialoguepage.h"

namespace Impacto {

using namespace Impacto::Profile::Dialogue;
using namespace Impacto::Profile::ScriptVars;

std::unique_ptr<WaitIcon> WaitIcon::Create(
    const DialoguePage* const parentPage) {
  switch (WaitIconCurrentType) {
    using enum WaitIconType;
    case None:
      return std::make_unique<VoidWaitIcon>(parentPage);

    case Sprite:
      return std::make_unique<SpriteWaitIcon>(parentPage, WaitIconSprite);

    case SpriteAnimation:
      return std::make_unique<SpriteAnimationWaitIcon>(
          parentPage, *WaitIconSpriteAnimationDef);

    case FixedSpriteAnimation:
      return std::make_unique<FixedSpriteAnimationWaitIcon>(
          parentPage, *WaitIconFixedSpriteAnimationDef);

    case Rotating:
      return std::make_unique<RotatingWaitIcon>(parentPage,
                                                WaitIconAnimationDuration);

    case Rotating3D:
      return std::make_unique<Rotating3DWaitIcon>(parentPage,
                                                  WaitIconAnimationDuration);
  }

  assert(false);
  return std::make_unique<VoidWaitIcon>(parentPage);
}

bool WaitIcon::IsVisible() const {
  if (GetFlag(SF_UIHIDDEN) || ((ScrWork[SW_GAMESTATE] & (1 << 2)) != 0)) {
    return false;
  }

  if (IsKeyIcon()) {
    return GetFlag(SF_KEYWAITICONDISP);
  }

  if (!GetFlag(SF_SHOWWAITICON + ParentPage->Id)) return false;
  if ((ScrWork[SW_GAMESTATE] & (1 << 7)) != 0) return false;
  if ((ScrWork[SW_MESWIN0TYPE +
               ParentPage->Id * Profile::Vm::ScrWorkMesStructSize] &
       +MesWinTypeBit::DontDrawWaitIcon) != 0) {
    return false;
  }
  if (ParentPage->GetTextModeInfo().WaitIconDispMode ==
      TextModeInfo::WaitIconDispModeType::Invisible) {
    return false;
  }

  return true;
}

void WaitIcon::Render(const glm::vec2 afterLastGlyphPos, const glm::vec4 tint) {
  const glm::vec2 pos = [&]() -> glm::vec2 {
    if (IsKeyIcon()) return KeyWaitIconPos;

    const TextModeInfo& pageInfo = ParentPage->GetTextModeInfo();
    switch (pageInfo.WaitIconDispMode) {
      using enum TextModeInfo::WaitIconDispModeType;
      case FixedPos:
        return pageInfo.WaitIconPos;
      case AfterLastGlyph:
        return afterLastGlyphPos + WaitIconOffset;
      case FixedXLineY:
        return {pageInfo.WaitIconPos.x, afterLastGlyphPos.y};

      case Invisible:
        break;
    }
    assert(false);
    return {0.0f, 0.0f};
  }();

  RenderImpl(pos, tint);
}

void FixedSpriteAnimationWaitIcon::Update(const float dt) {
  const bool isVisible = IsVisible();
  if (isVisible && IconAnimation.IsOut()) {
    IconAnimation.StartIn();
  } else if (!isVisible && IconAnimation.IsIn()) {
    IconAnimation.StartOut();
  }

  IconAnimation.Update(dt);
}

void RotatingWaitIcon::RenderImpl(const glm::vec2 pos, const glm::vec4 tint) {
  if (!IsVisible()) return;

  const CornersQuad dest =
      WaitIconSprite.ScaledBounds()
          .RotateAroundCenter(RotationAnimation.Progress * 2.0f *
                              std::numbers::pi_v<float>)
          .Translate(pos);
  Renderer->DrawSprite(WaitIconSprite, dest, tint);
}

void Rotating3DWaitIcon::RenderImpl(const glm::vec2 pos, const glm::vec4 tint) {
  if (!IsVisible()) return;

  // TODO: MO6TW only for now
  const glm::vec3 euler(
      RotationAnimation.Progress * 2.0f * std::numbers::pi_v<float>, 0, 0.6f);
  glm::quat quat;
  eulerZYXToQuat(&euler, &quat);

  const glm::vec2 vanishingPoint = pos + WaitIconSprite.Center();

  CornersQuad dest = WaitIconSprite.ScaledBounds().Translate(pos);
  dest.Rotate(quat, {dest.Center(), 0.0f}, 1.0f, vanishingPoint, true);
  Renderer->DrawSprite(WaitIconSprite, dest, tint);
}

}  // namespace Impacto
