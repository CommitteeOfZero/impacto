#pragma once

#include "../button.h"
#include "../../../games/mo6tw/tipsmenu.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

int const TipNumberLength = 4;
int const TipLockedTextLength = 3;
int const NewTextLength = 3;

class TipsEntryButton : public Widgets::Button {
 public:
  TipsEntryButton(int id, UI::MO6TW::TipsDataRecord* tipRecord,
                  RectF const& dest, Sprite const& highlight);
  void Render() override;

 private:
  UI::MO6TW::TipsDataRecord* TipEntryRecord;
  ProcessedTextGlyph TipNumber[TipNumberLength];
  ProcessedTextGlyph TipName[255];
  ProcessedTextGlyph TipLockedText[TipLockedTextLength];
  ProcessedTextGlyph NewText[NewTextLength];

  int TipNameLength;
};

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto