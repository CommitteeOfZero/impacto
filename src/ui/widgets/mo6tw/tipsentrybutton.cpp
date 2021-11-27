#include "tipsentrybutton.h"

#include "../../../renderer2d.h"
#include "../../../profile/dialogue.h"
#include "../../../text.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

TipsEntryButton::TipsEntryButton(int id, UI::MO6TW::TipsDataRecord* tipRecord,
                                 RectF const& dest, Sprite const& highlight) {
  Id = id;
  TipEntryRecord = tipRecord;
  Bounds = dest;
  HighlightSprite = highlight;
  Enabled = true;
  HighlightOffset = glm::vec2(0.0f, -9.0f);
  char tipNumber[5];
  sprintf(tipNumber, "%3d.", tipRecord->id + 1);
  TextLayoutPlainString(std::string(tipNumber), TipNumber,
                        Profile::Dialogue::DialogueFont, 20,
                        Profile::Dialogue::ColorTable[0], 1.0f,
                        glm::vec2(Bounds.X, Bounds.Y), TextAlignment::Left);
  Vm::Sc3VmThread dummy;
  dummy.Ip = tipRecord->stringPtrs[0];
  TipNameLength = TextLayoutPlainLine(
      &dummy, 255, TipName, Profile::Dialogue::DialogueFont, 20,
      Profile::Dialogue::ColorTable[0], 1.0f,
      glm::vec2(Bounds.X + 50.0f, Bounds.Y), TextAlignment::Left);
  TextLayoutPlainString(std::string("New"), NewText,
                        Profile::Dialogue::DialogueFont, 20,
                        Profile::Dialogue::ColorTable[0], 1.0f,
                        glm::vec2(Bounds.X, Bounds.Y), TextAlignment::Left);
}

void TipsEntryButton::Render() {
  if (HasFocus) {
    Renderer2D::DrawSprite(
        HighlightSprite,
        glm::vec2(Bounds.X + HighlightOffset.x, Bounds.Y + HighlightOffset.y),
        Tint, glm::vec2(Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f));
  }

  Renderer2D::DrawProcessedText(TipNumber, TipNumberLength,
                                Profile::Dialogue::DialogueFont, 1.0f, true);
  Renderer2D::DrawProcessedText(TipName, TipNameLength,
                                Profile::Dialogue::DialogueFont, 1.0f, true);
}

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto