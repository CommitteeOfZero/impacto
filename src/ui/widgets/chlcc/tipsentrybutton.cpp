#include "tipsentrybutton.h"

#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/games/chlcc/tipsmenu.h"
#include "../../../text.h"
#include "../../../vm/vm.h"
#include "../../../inputsystem.h"
#include "../../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

using namespace Impacto::TipsSystem;

using namespace Impacto::Profile::CHLCC::TipsMenu;
TipsEntryButton::TipsEntryButton(int id, TipsDataRecord* tipRecord,
                                 RectF const& dest, Sprite const& highlight)
    : TipEntryRecord(tipRecord), PrevUnreadState(tipRecord->IsUnread) {
  Id = id;
  Bounds = dest;
  HighlightSprite = highlight;
  Enabled = true;
  HasText = true;
  HighlightOffset = {0.0f, 0.0f};
  TextLayoutPlainString(fmt::format("{:3d}.", id + 1), TipNumber,
                        Profile::Dialogue::DialogueFont, TipListEntryFontSize,
                        Profile::Dialogue::ColorTable[DefaultColorIndex], 1.0f,
                        glm::vec2(Bounds.X + TipListEntryNameXOffset - 5.0f +
                                      TipListEntryTextOffsetX,
                                  Bounds.Y),
                        TextAlignment::Right);

  Vm::Sc3VmThread dummy;
  dummy.IpOffset = tipRecord->StringAdr[0];
  dummy.ScriptBufferId = TipsSystem::GetTipsScriptBufferId();
  auto textColorIndex =
      (tipRecord->IsUnread) ? UnreadColorIndex : DefaultColorIndex;
  Text = TextLayoutPlainLine(
      &dummy, 255, Profile::Dialogue::DialogueFont, TipListEntryFontSize,
      Profile::Dialogue::ColorTable[textColorIndex], 1.0f,
      glm::vec2(Bounds.X + TipListEntryNameXOffset + TipListEntryTextOffsetX,
                Bounds.Y),
      TextAlignment::Left);

  auto lockedScrPos =
      Vm::ScriptGetTextTableStrAddress(TipsStringTable, LockedTipsIndex);
  dummy.IpOffset = lockedScrPos.IpOffset;
  dummy.ScriptBufferId = lockedScrPos.ScriptBufferId;
  TextLayoutPlainLine(
      &dummy, 3, TipLockedText, Profile::Dialogue::DialogueFont,
      TipListEntryFontSize, Profile::Dialogue::ColorTable[UnreadColorIndex],
      1.0f,
      glm::vec2(Bounds.X + TipListEntryNameXOffset + TipListEntryTextOffsetX,
                Bounds.Y),
      TextAlignment::Left);
}

void TipsEntryButton::Move(glm::vec2 relativePos) {
  Button::Move(relativePos);
  for (size_t i = 0; i < TipNumber.size(); i++) {
    TipNumber[i].DestRect += relativePos;
  }
  for (size_t i = 0; i < TipLockedText.size(); i++) {
    TipLockedText[i].DestRect += relativePos;
  }
}

void TipsEntryButton::Update(float dt) {
  Button::Update(dt);
  if (PrevUnreadState != TipEntryRecord->IsUnread) {
    int colorIndex = DefaultColorIndex;
    if (TipEntryRecord->IsUnread) {
      colorIndex = UnreadColorIndex;
    }
    for (ProcessedTextGlyph& glyph : Text) {
      glyph.Colors = Profile::Dialogue::ColorTable[colorIndex];
    }
    PrevUnreadState = TipEntryRecord->IsUnread;
  }
}

void TipsEntryButton::Render() {
  if (HasFocus) {
    Renderer->DrawSprite(HighlightSprite, Bounds.GetPos() + HighlightOffset,
                         Tint);
    Renderer->DrawSprite(
        TipsEntryHighlightDot,
        Bounds.GetPos() + HighlightOffset + TipsListEntryDotOffset, Tint);
  }

  Renderer->DrawProcessedText(TipNumber, Profile::Dialogue::DialogueFont,
                              Tint.a, RendererOutlineMode::Full);
  if (TipEntryRecord->IsLocked) {
    Renderer->DrawProcessedText(TipLockedText, Profile::Dialogue::DialogueFont,
                                Tint.a, RendererOutlineMode::Full);
  } else {
    Renderer->DrawProcessedText(Text, Profile::Dialogue::DialogueFont, Tint.a,
                                RendererOutlineMode::Full);
    if (TipEntryRecord->IsNew) {
      Renderer->DrawSprite(TipsEntryNewDot,
                           Bounds.GetPos() + TipsListNewDotOffset, Tint);
    }
  }
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto