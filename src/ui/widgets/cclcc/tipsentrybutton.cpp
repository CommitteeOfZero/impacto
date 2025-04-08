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
  TextLayoutPlainString(fmt::format("{:03d}", dispId), TipNumber,
                        Profile::Dialogue::DialogueFont, 21,
                        {TipsMenuDarkTextColor, 0}, 1.0f,
                        glm::vec2(Bounds.X, Bounds.Y) + TipsEntryNumberOffset,
                        TextAlignment::Left);
  Vm::Sc3VmThread dummy;
  glm::vec2 nameDest = glm::vec2(Bounds.X, Bounds.Y) + TipsEntryNameOffset;
  dummy.Ip = TipEntryRecord->StringPtrs[1];
  HasText = true;

  uint32_t initColorName =
      PrevUnreadState ? TipsEntryNameUnreadColor : TipsMenuDarkTextColor;
  Text = TextLayoutPlainLine(&dummy, 255, Profile::Dialogue::DialogueFont, 26,
                             {initColorName, 0}, 1.0f, nameDest,
                             TextAlignment::Left);

  dummy.Ip = Vm::ScriptGetStrAddress(
      Impacto::Vm::ScriptBuffers[TipsSystem::GetTipsScriptBufferId()],
      TipsTextEntryLockedIndex);
  TextLayoutPlainLine(&dummy, TipLockedTextLength, TipLockedText,
                      Profile::Dialogue::DialogueFont, 26, {initColorName, 0},
                      1.0f, nameDest, TextAlignment::Left);
  HighlightOffset = TipsEntryHighlightOffset;
  Bounds = dest;
}

void TipsEntryButton::Update(float dt) {
  Button::Update(dt);
  bool curUnreadState = TipEntryRecord->IsUnread && !TipEntryRecord->IsLocked;
  if (PrevUnreadState != curUnreadState) {
    uint32_t colorName =
        curUnreadState ? TipsEntryNameUnreadColor : TipsMenuDarkTextColor;
    for (int i = 0; i < Text.size(); i++) {
      Text[i].Colors = {colorName, 0};
    }
    PrevUnreadState = curUnreadState;
  }
  if (PrevFocusState != HasFocus) {
    PrevFocusState = HasFocus;
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0);
  }
}

void TipsEntryButton::UpdateInput() {
  if (TipsTabBounds.Intersects(Bounds) || TipsTabBounds.Contains(Bounds)) {
    Button::UpdateInput();
  }
}

void TipsEntryButton::Render() {
  if (HasFocus) {
    Renderer->DrawSprite(
        HighlightSprite,
        glm::vec2(Bounds.X + HighlightOffset.x, Bounds.Y + HighlightOffset.y),
        Tint, glm::vec2(Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f));
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
  for (int i = 0; i < TipNumberLength; i++) {
    TipNumber[i].DestRect.X += relativePos.x;
    TipNumber[i].DestRect.Y += relativePos.y;
  }
  for (int i = 0; i < TipLockedTextLength; i++) {
    TipLockedText[i].DestRect.X += relativePos.x;
    TipLockedText[i].DestRect.Y += relativePos.y;
  }
}

void TipsEntryButton::MoveTo(glm::vec2 pos) {
  auto relativePos = pos - glm::vec2(Bounds.X, Bounds.Y);
  Move(relativePos);
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto