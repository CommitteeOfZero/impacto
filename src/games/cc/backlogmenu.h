#pragma once

#include "../../ui/backlogmenu.h"
#include "../../ui/widgets/cc/backlogentry.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace CC {

class BacklogMenu : public UI::BacklogMenu {
 public:
  void Show() override;
  void Hide() override;
  void Render() override;

  Widgets::BacklogEntry* CreateBacklogEntry(
      int id, Vm::BufferOffsetContext scrCtx, int audioId, int characterId,
      glm::vec2 pos, const RectF& hoverBounds) const override {
    return new Widgets::CC::BacklogEntry(id, scrCtx, audioId, characterId, pos,
                                         hoverBounds);
  }

  void MenuButtonOnClick(Widgets::BacklogEntry* target) override;

 private:
  void UpdateVisibility() override;
};

}  // namespace CC
}  // namespace UI
}  // namespace Impacto