#pragma once

#include "../button.h"
#include "../../../data/tipssystem.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {
class TipsEntryButton : public Button {
 public:
  TipsEntryButton(int id, TipsSystem::TipsDataRecord* tipRecord,
                  RectF const& dest, Sprite const& highlight);
  void Render() override;
  void Update(float dt) override;
  void Move(glm::vec2 relativePos) override;

  TipsSystem::TipsDataRecord* TipEntryRecord;

 private:
  std::array<ProcessedTextGlyph, 4> TipNumber;
  std::array<ProcessedTextGlyph, 4> TipLockedText;
  bool PrevUnreadState;
};

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto