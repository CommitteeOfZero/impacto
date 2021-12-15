#include "tipsentrybutton.h"

#include "../../../renderer2d.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/games/mo6tw/tipsmenu.h"
#include "../../../text.h"
#include "../../../vm/vm.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

using namespace Impacto::TipsSystem;

using namespace Impacto::Profile::MO6TW::TipsMenu;

TipsEntryButton::TipsEntryButton(int id, TipsDataRecord* tipRecord,
                                 RectF const& dest, Sprite const& highlight) {
  Id = id;
  TipEntryRecord = tipRecord;
  Bounds = dest;
  HighlightSprite = highlight;
  Enabled = true;
  HighlightOffset = TipListEntryHighlightOffset;
  PrevUnreadState = TipEntryRecord->IsUnread;
  char tipNumber[5];
  sprintf(tipNumber, "%3d.", tipRecord->Id + 1);
  TextLayoutPlainString(std::string(tipNumber), TipNumber,
                        Profile::Dialogue::DialogueFont, TipListEntryFontSize,
                        Profile::Dialogue::ColorTable[DefaultColorIndex], 1.0f,
                        glm::vec2(Bounds.X, Bounds.Y), TextAlignment::Left);
  Vm::Sc3VmThread dummy;
  dummy.Ip = tipRecord->StringPtrs[0];
  TipNameLength = TextLayoutPlainLine(
      &dummy, 255, TipName, Profile::Dialogue::DialogueFont,
      TipListEntryFontSize, Profile::Dialogue::ColorTable[DefaultColorIndex],
      1.0f, glm::vec2(Bounds.X + TipListEntryNameXOffset, Bounds.Y),
      TextAlignment::Left);
  TextLayoutPlainString(TipListEntryNewText, NewText,
                        Profile::Dialogue::DialogueFont, TipListEntryFontSize,
                        Profile::Dialogue::ColorTable[DefaultColorIndex], 1.0f,
                        glm::vec2(Bounds.X + TipListEntryNewOffset, Bounds.Y),
                        TextAlignment::Left);
  dummy.Ip = Vm::ScriptGetTextTableStrAddress(TipListEntryLockedTable,
                                              TipListEntryLockedIndex);
  TextLayoutPlainLine(&dummy, 255, TipLockedText,
                      Profile::Dialogue::DialogueFont, TipListEntryFontSize,
                      Profile::Dialogue::ColorTable[UnreadColorIndex], 1.0f,
                      glm::vec2(Bounds.X + TipListEntryNameXOffset, Bounds.Y),
                      TextAlignment::Left);
}

void TipsEntryButton::Update(float dt) {
  Button::Update(dt);
  if (PrevUnreadState != TipEntryRecord->IsUnread) {
    int colorIndex = DefaultColorIndex;
    if (TipEntryRecord->IsUnread) {
      colorIndex = UnreadColorIndex;
    }
    for (int i = 0; i < TipNameLength; i++) {
      TipName[i].Colors = Profile::Dialogue::ColorTable[colorIndex];
    }
    PrevUnreadState = TipEntryRecord->IsUnread;
  }
}

void TipsEntryButton::Render() {
  if (HasFocus) {
    Renderer2D::DrawSprite(
        HighlightSprite,
        glm::vec2(Bounds.X + HighlightOffset.x, Bounds.Y + HighlightOffset.y),
        Tint, glm::vec2(Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f));
  }

  Renderer2D::DrawProcessedText(TipNumber, TipNumberLength,
                                Profile::Dialogue::DialogueFont, Tint.a, true);
  if (TipEntryRecord->IsLocked) {
    Renderer2D::DrawProcessedText(TipLockedText, TipLockedTextLength,
                                  Profile::Dialogue::DialogueFont, Tint.a,
                                  true);
  } else {
    Renderer2D::DrawProcessedText(
        TipName, TipNameLength, Profile::Dialogue::DialogueFont, Tint.a, true);
    if (TipEntryRecord->IsNew) {
      Renderer2D::DrawProcessedText(NewText, NewTextLength,
                                    Profile::Dialogue::DialogueFont, Tint.a,
                                    true);
    }
  }
}

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto