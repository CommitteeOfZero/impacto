#pragma once

#include "menu.h"
#include "turboonholdhandler.h"
#include "widgets/group.h"
#include "widgets/backlogentry.h"
#include "widgets/scrollbar.h"

namespace Impacto {
namespace UI {

class BacklogMenu : public Menu {
 public:
  BacklogMenu();

  virtual void Show() override;
  virtual void Hide() override;
  virtual void Update(float dt) override;
  virtual void UpdateInput(float dt) override;
  virtual void Render() override;

  virtual Widgets::BacklogEntry* CreateBacklogEntry(
      int id, Vm::BufferOffsetContext scrCtx, int audioId, int characterId,
      glm::vec2 pos, const RectF& hoverBounds) const {
    return new Widgets::BacklogEntry(id, scrCtx, audioId, characterId, pos,
                                     hoverBounds);
  }

  virtual void AddMessage(Vm::BufferOffsetContext scrCtx, int audioId = -1,
                          int characterId = 0);
  virtual void MenuButtonOnClick(Widgets::BacklogEntry* target);
  void Clear();

  float PageY = 0.0f;

 protected:
  virtual void UpdateVisibility() {};

  int CurrentId = 0;
  float ItemsHeight = 0.0f;
  glm::vec2 CurrentEntryPos;
  Widgets::Group* MainItems;
  Animation FadeAnimation;
  Widgets::Scrollbar* MainScrollbar;

  TurboOnHoldHandler DirectionButtonHeldHandler;
  TurboOnHoldHandler PageUpDownButtonHeldHandler;

  void RenderHighlight(glm::vec2 offset = {0.0f, 0.0f}) const;

  void UpdatePageUpDownInput(float dt);
  void UpdateScrollingInput(float dt);
};

}  // namespace UI
}  // namespace Impacto