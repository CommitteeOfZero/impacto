#pragma once

#include "../button.h"
#include "../../../data/tipssystem.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

class TipsEntryButton : public Widgets::Button {
 public:
  TipsEntryButton(int id, Impacto::TipsSystem::TipsDataRecord* tipRecord,
                  RectF const& dest, Sprite const& highlight);
  void Update(float dt) override;
  void Render() override;

  Impacto::TipsSystem::TipsDataRecord* TipEntryRecord;

 private:
  std::array<ProcessedTextGlyph, 3> TipNumber;
  std::array<ProcessedTextGlyph, 3> TipLockedText;
  std::array<ProcessedTextGlyph, 3> NewText;

  bool PrevUnreadState;
};

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto