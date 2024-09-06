#pragma once

#include "../button.h"
#include "../../../data/tipssystem.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

int constexpr TipNumberLength = 4;
int constexpr TipLockedTextLength = 3;
int constexpr NewTextLength = 3;

class TipsEntryButton : public Widgets::Button {
 public:
  TipsEntryButton(int id, Impacto::TipsSystem::TipsDataRecord* tipRecord,
                  RectF const& dest, Sprite const& highlight);
  void Update(float dt) override;
  void Render() override;

  Impacto::TipsSystem::TipsDataRecord* TipEntryRecord;

 private:
  std::array<ProcessedTextGlyph, TipNumberLength> TipNumber;
  std::array<ProcessedTextGlyph, TipLockedTextLength> TipLockedText;
  std::array<ProcessedTextGlyph, NewTextLength> NewText;

  bool PrevUnreadState;
};

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto