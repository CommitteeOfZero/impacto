#pragma once

#include "../../ui/backlogmenu.h"
#include "../../ui/widgets/chlcc/backlogentry.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class BacklogMenu : public UI::BacklogMenu {
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

 private:
  void DrawCircles();
  void DrawErin();
  void DrawRedBar();
  void DrawButtonPrompt();

  void UpdateTitles();

  Animation MenuTransition;
  Animation TitleFade;
  Animation FromSystemMenuTransition;

  glm::vec2 RedTitleLabelPos;
  glm::vec2 RightTitlePos;
  glm::vec2 LeftTitlePos;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto