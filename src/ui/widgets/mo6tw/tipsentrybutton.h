#pragma once

#include "../button.h"
#include "../../../data/tipssystem.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

int const TipNumberLength = 4;
int const TipLockedTextLength = 3;
int const NewTextLength = 3;

class TipsEntryButton : public Widgets::Button {
 public:
  TipsEntryButton(int id, Impacto::TipsSystem::TipsDataRecord* tipRecord,
                  RectF const& dest, Sprite const& highlight);
  void Update(float dt) override;
  void Render() override;

  Impacto::TipsSystem::TipsDataRecord* TipEntryRecord;

 private:
  ProcessedTextGlyph TipNumber[TipNumberLength];
  ProcessedTextGlyph TipName[255];
  ProcessedTextGlyph TipLockedText[TipLockedTextLength];
  ProcessedTextGlyph NewText[NewTextLength];

  int TipNameLength;
  bool PrevUnreadState;
};

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto