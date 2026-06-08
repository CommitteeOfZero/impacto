#pragma once

#include "../cclcc/commonmenu.h"
#include "../../ui/backlogmenu.h"
#include "../../ui/widgets/cc/backlogentry.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace CC {

class BacklogMenu : public UI::BacklogMenu, public CCLCC::CommonMenu {
 public:
  void Show() override;
  void Hide() override;
  void Render() override;
  void Update(float dt) override;

  Widgets::BacklogEntry* CreateBacklogEntry(
      Vm::BufferOffsetContext scrCtx, std::optional<int> audioId,
      int characterId, glm::vec2 pos, const RectF& hoverBounds) const override {
    return new Widgets::CC::BacklogEntry(scrCtx, audioId, characterId, pos,
                                         hoverBounds);
  }

  void MenuButtonOnClick(Widgets::BacklogEntry* target) override;

 private:
  void UpdateVisibility() override;
};

}  // namespace CC
}  // namespace UI
}  // namespace Impacto