#pragma once

#include "commonmenu.h"
#include "../../ui/backlogmenu.h"
#include "../../ui/widgets/chlcc/backlogentry.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class BacklogMenu : public UI::BacklogMenu, public CommonMenu {
 public:
  BacklogMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void Render() override;

  Widgets::BacklogEntry* CreateBacklogEntry(
      int id, Vm::BufferOffsetContext scrCtx, int audioId, int characterId,
      glm::vec2 pos, const RectF& hoverBounds) const override {
    return new Widgets::CHLCC::BacklogEntry(id, scrCtx, audioId, characterId,
                                            pos, hoverBounds);
  }
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto