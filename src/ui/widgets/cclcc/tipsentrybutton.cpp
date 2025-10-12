#include "tipsentrybutton.h"

#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/games/cclcc/tipsmenu.h"
#include "../../../text.h"
#include "../../../vm/vm.h"
#include "../../../inputsystem.h"
#include "../../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

using namespace Impacto::TipsSystem;

using namespace Impacto::Profile::CCLCC::TipsMenu;

TipsEntryButton::TipsEntryButton(int tipId, int dispId, RectF const& dest,
                                 Sprite const& highlight, bool isNew)
    : Button(dispId, Sprite(), highlight, highlight, {dest.X, dest.Y}),
      IsNewState(isNew) {
  Id = dispId;
  TipEntryRecord = TipsSystem::GetTipRecord(tipId);
  Enabled = true;
  HighlightOffset = {0, 0};
  PrevUnreadState = TipEntryRecord->IsUnread && !TipEntryRecord->IsLocked;
  TextLayoutPlainString(fmt::format("{:03d}.", dispId), TipNumber,
                        Profile::Dialogue::DialogueFont,
                        (float)TipsEntryNumberFontSize,
                        {TipsMenuDarkTextColor, 0}, 1.0f,
                        glm::vec2(Bounds.X, Bounds.Y) + TipsEntryNumberOffset,
                        TextAlignment::Left);
  Vm::Sc3VmThread dummy;
  dummy.ScriptBufferId = TipsSystem::GetTipsScriptBufferId();
  glm::vec2 nameDest = glm::vec2(Bounds.X, Bounds.Y) + TipsEntryNameOffset;
  dummy.IpOffset = TipEntryRecord->StringAdr[1];
  HasText = true;

  uint32_t initColorName =
      PrevUnreadState ? TipsEntryNameUnreadColor : TipsMenuDarkTextColor;
  Text = TextLayoutPlainLine(&dummy, 255, Profile::Dialogue::DialogueFont,
                             (float)TipsEntryNameFontSize, {initColorName, 0},
                             1.0f, nameDest, TextAlignment::Left);

  dummy.IpOffset = Vm::ScriptGetStrAddress(TipsSystem::GetTipsScriptBufferId(),
                                           TipsTextEntryLockedIndex);
  TextLayoutPlainLine(&dummy, static_cast<int>(TipLockedText.size()),
                      TipLockedText, Profile::Dialogue::DialogueFont,
                      (float)TipsEntryNameFontSize, {initColorName, 0}, 1.0f,
                      nameDest, TextAlignment::Left);
  HighlightOffset = TipsEntryHighlightOffset;
  Bounds = dest;
  HoverBounds = dest;
}

void TipsEntryButton::Update(float dt) {
  Button::Update(dt);
  bool curUnreadState = TipEntryRecord->IsUnread && !TipEntryRecord->IsLocked;
  if (PrevUnreadState != curUnreadState) {
    uint32_t colorName =
        curUnreadState ? TipsEntryNameUnreadColor : TipsMenuDarkTextColor;
    for (ProcessedTextGlyph& glyph : Text) {
      glyph.Colors = {colorName, 0};
    }
    PrevUnreadState = curUnreadState;
  }
  if (!PrevFocusState && HasFocus)
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0);
  if (PrevFocusState != HasFocus) {
    PrevFocusState = HasFocus;
  }
}

void TipsEntryButton::UpdateInput(float dt) {
  if (TipsTabBounds.Intersects(Bounds) || TipsTabBounds.Contains(Bounds)) {
    Button::UpdateInput(dt);
  }
}

void TipsEntryButton::Render() {
  if (HasFocus) {
    const RectF dest =
        HighlightSprite.ScaledBounds()
            .Scale({Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f},
                   {0.0f, 0.0f})
            .Translate(Bounds.GetPos() + HighlightOffset);
    Renderer->DrawSprite(HighlightSprite, dest, Tint);
  }

  Renderer->DrawProcessedText(TipNumber, Profile::Dialogue::DialogueFont,
                              Tint.a, RendererOutlineMode::None);
  if (TipEntryRecord->IsLocked) {
    Renderer->DrawProcessedText(TipLockedText, Profile::Dialogue::DialogueFont,
                                Tint.a, RendererOutlineMode::None);
  } else {
    Renderer->DrawProcessedText(Text, Profile::Dialogue::DialogueFont, Tint.a,
                                RendererOutlineMode::None);
    if (IsNewState) {
      Renderer->DrawSprite(TipsNewSprite,
                           glm::vec2{Bounds.X, Bounds.Y} + TipEntryNewOffset,
                           Tint);
    }
  }
}

void TipsEntryButton::Move(glm::vec2 relativePos) {
  Button::Move(relativePos);
  for (size_t i = 0; i < TipNumber.size(); i++) {
    TipNumber[i].DestRect.X += relativePos.x;
    TipNumber[i].DestRect.Y += relativePos.y;
  }
  for (size_t i = 0; i < TipLockedText.size(); i++) {
    TipLockedText[i].DestRect.X += relativePos.x;
    TipLockedText[i].DestRect.Y += relativePos.y;
  }
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto