#pragma once

#include "../button.h"
#include "../../../data/tipssystem.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

int constexpr TipNumberLength = 4;
int constexpr TipLockedTextLength = 10;

class TipsEntryButton : public Widgets::Button {
 public:
  TipsEntryButton(int tipId, int dispId, RectF const& dest,
                  Sprite const& highlight, bool isNew);
  void Update(float dt) override;
  void UpdateInput() override;
  void Render() override;
  void Move(glm::vec2 pos) override;
  void MoveTo(glm::vec2 pos) override;

  TipsSystem::TipsDataRecord const* TipEntryRecord;

 private:
  std::array<ProcessedTextGlyph, TipNumberLength> TipNumber;
  std::array<ProcessedTextGlyph, TipLockedTextLength> TipLockedText;
  bool PrevUnreadState;
  bool IsNewState;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto