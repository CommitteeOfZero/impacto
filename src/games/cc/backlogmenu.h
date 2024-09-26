#pragma once

#include "../../ui/backlogmenu.h"
#include "../../ui/widgets/cc/backlogentry.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace CC {

class BacklogMenu : public UI::BacklogMenu {
 public:
  BacklogMenu();

  void Render();
  void MenuButtonOnClick(Widgets::BacklogEntry* target);

 private:
  const Sprite* MaskSprite;
  Widgets::CC::BacklogEntry* CreateBacklogEntry(
      int id, uint8_t* str, int audioId, glm::vec2 pos,
      const RectF& hoverBounds) const override;
};

}  // namespace CC
}  // namespace UI
}  // namespace Impacto