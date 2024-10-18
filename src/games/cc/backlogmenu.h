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
  void Update(float dt) override;
  void Render() override;

  void MenuButtonOnClick(Widgets::BacklogEntry* target) override;

 private:
  Widgets::CC::BacklogEntry* CreateBacklogEntry(
      int id, uint8_t* str, int audioId, glm::vec2 pos,
      const RectF& hoverBounds) const override;
};

}  // namespace CC
}  // namespace UI
}  // namespace Impacto