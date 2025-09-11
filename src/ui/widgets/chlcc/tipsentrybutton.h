#pragma once

#include "../button.h"
#include "../../../data/tipssystem.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

int constexpr inline TipNumberLength = 4;
int constexpr inline TipLockedTextLength = 4;

class TipsEntryButton : public Button {
 public:
  TipsEntryButton(int id, TipsSystem::TipsDataRecord* tipRecord,
                  RectF const& dest, Sprite const& highlight);
  void Render() override;
  void Update(float dt) override;
  void Move(glm::vec2 relativePos) override;
  void MoveTo(glm::vec2 pos) override;

  TipsSystem::TipsDataRecord const* TipEntryRecord;

 private:
  std::array<ProcessedTextGlyph, TipNumberLength> TipNumber;
  std::array<ProcessedTextGlyph, TipLockedTextLength> TipLockedText;
  bool PrevUnreadState;
  bool IsNewState;
};

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto