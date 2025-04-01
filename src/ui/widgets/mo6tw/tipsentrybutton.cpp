#include "tipsentrybutton.h"

#include "../../../renderer/renderer.h"
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
  TextLayoutPlainString(fmt::format("{:3d}.", tipRecord->Id + 1), TipNumber,
                        Profile::Dialogue::DialogueFont, TipListEntryFontSize,
                        Profile::Dialogue::ColorTable[DefaultColorIndex], 1.0f,
                        glm::vec2(Bounds.X, Bounds.Y), TextAlignment::Left);
  Vm::Sc3VmThread dummy;
  dummy.Ip = tipRecord->StringPtrs[0];
  Text = TextLayoutPlainLine(
      &dummy, 255, Profile::Dialogue::DialogueFont, TipListEntryFontSize,
      Profile::Dialogue::ColorTable[DefaultColorIndex], 1.0f,
      glm::vec2(Bounds.X + TipListEntryNameXOffset, Bounds.Y),
      TextAlignment::Left);
  TextLayoutPlainString(TipListEntryNewText, NewText,
                        Profile::Dialogue::DialogueFont, TipListEntryFontSize,
                        Profile::Dialogue::ColorTable[DefaultColorIndex], 1.0f,
                        glm::vec2(Bounds.X + TipListEntryNewOffset, Bounds.Y),
                        TextAlignment::Left);
  dummy.Ip = Vm::ScriptGetTextTableStrAddress(TipListEntryLockedTable,
                                              TipListEntryLockedIndex);
  TextLayoutPlainLine(&dummy, 3, TipLockedText, Profile::Dialogue::DialogueFont,
                      TipListEntryFontSize,
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
    for (int i = 0; i < Text.size(); i++) {
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

  Renderer->DrawProcessedText(TipNumber, Profile::Dialogue::DialogueFont,
                              Tint.a, RendererOutlineMode::RO_Full);
  if (TipEntryRecord->IsLocked) {
    Renderer->DrawProcessedText(TipLockedText, Profile::Dialogue::DialogueFont,
                                Tint.a, RendererOutlineMode::RO_Full);
  } else {
    Renderer->DrawProcessedText(Text, Profile::Dialogue::DialogueFont, Tint.a,
                                RendererOutlineMode::RO_Full);
    if (TipEntryRecord->IsNew) {
      Renderer->DrawProcessedText(NewText, Profile::Dialogue::DialogueFont,
                                  Tint.a, RendererOutlineMode::RO_Full);
    }
  }
}

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto