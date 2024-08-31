#include "tipsentrybutton.h"

#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/games/cclcc/tipsmenu.h"
#include "../../../text.h"
#include "../../../vm/vm.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

using namespace Impacto::TipsSystem;

using namespace Impacto::Profile::CCLCC::TipsMenu;

TipsEntryButton::TipsEntryButton(int tipId, int dispId, RectF const& dest,
                                 Sprite const& highlight)
    : Button(dispId, Sprite(), highlight, highlight, {dest.X, dest.Y}) {
  Id = dispId;
  TipEntryRecord = TipsSystem::GetTipRecord(tipId);
  Enabled = true;
  HighlightOffset = {0, 0};
  PrevUnreadState = TipEntryRecord->IsUnread;
  char tipNumber[5];
  sprintf(tipNumber, "%03d.", dispId);
  TextLayoutPlainString(std::string(tipNumber), TipNumber.data(),
                        Profile::Dialogue::DialogueFont, 21,
                        Profile::Dialogue::ColorTable[10], 1.0f,
                        glm::vec2(Bounds.X, Bounds.Y) + TipsEntryNumberOffset,
                        TextAlignment::Left);
  Vm::Sc3VmThread dummy;
  glm::vec2 nameDest = glm::vec2(Bounds.X, Bounds.Y) + TipsEntryNameOffset;
  dummy.Ip = TipEntryRecord->StringPtrs[1];
  HasText = true;
  Text = new ProcessedTextGlyph[255];
  TextLength = TextLayoutPlainLine(
      &dummy, 255, Text, Profile::Dialogue::DialogueFont, 26,
      Profile::Dialogue::ColorTable[10], 1.0f, nameDest, TextAlignment::Left);

  dummy.Ip = Vm::ScriptGetStrAddress(
      Impacto::Vm::ScriptBuffers[TipsSystem::GetTipsScriptBufferId()],
      TipsTextEntryLockedIndex);
  TextLayoutPlainLine(&dummy, TipLockedTextLength, TipLockedText.data(),
                      Profile::Dialogue::DialogueFont, 26,
                      Profile::Dialogue::ColorTable[10], 1.0f, nameDest,
                      TextAlignment::Left);
  HighlightOffset = TipsEntryHighlightOffset;
  Bounds = dest;
}

void TipsEntryButton::Update(float dt) {
  Button::Update(dt);
  if (PrevUnreadState != TipEntryRecord->IsUnread) {
    int colorIndex = 10;
    if (TipEntryRecord->IsUnread) {
      colorIndex = 7;
    }
    for (int i = 0; i < TextLength; i++) {
      Text[i].Colors = Profile::Dialogue::ColorTable[colorIndex];
    }
    PrevUnreadState = TipEntryRecord->IsUnread;
  }
}

void TipsEntryButton::Render() {
  if (HasFocus) {
    Renderer->DrawSprite(
        HighlightSprite,
        glm::vec2(Bounds.X + HighlightOffset.x, Bounds.Y + HighlightOffset.y),
        Tint, glm::vec2(Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f));
  }

  Renderer->DrawProcessedText(TipNumber.data(), TipNumberLength,
                              Profile::Dialogue::DialogueFont, Tint.a,
                              RendererOutlineMode::RO_Full);
  if (TipEntryRecord->IsLocked) {
    Renderer->DrawProcessedText(TipLockedText.data(), TipLockedTextLength,
                                Profile::Dialogue::DialogueFont, Tint.a,
                                RendererOutlineMode::RO_Full);
  } else {
    Renderer->DrawProcessedText(Text, TextLength,
                                Profile::Dialogue::DialogueFont, Tint.a,
                                RendererOutlineMode::RO_Full);
    if (TipEntryRecord->IsNew) {
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