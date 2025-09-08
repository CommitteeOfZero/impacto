#pragma once

#include "../button.h"
#include "../../../data/tipssystem.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

int constexpr TipLockedTextLength = 5;

class TipsEntryButton : public Widgets::Button {
 public:
  TipsEntryButton(int tipId, int dispId, RectF const& dest,
                  Sprite const& highlight, bool isNew);
  void Update(float dt) override;
  void UpdateInput(float dt) override;
  void Render() override;

  using Widget::Move;
  void Move(glm::vec2 pos) override;

  TipsSystem::TipsDataRecord const* TipEntryRecord;
  bool PrevFocusState = false;

 private:
  std::array<ProcessedTextGlyph, 4> TipNumber;
  std::array<ProcessedTextGlyph, 5> TipLockedText;
  bool PrevUnreadState;
  bool IsNewState;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto