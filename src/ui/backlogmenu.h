#pragma once

#include "menu.h"
#include "widgets/group.h"
#include "widgets/backlogentry.h"
#include "widgets/scrollbar.h"

namespace Impacto {
namespace UI {

class BacklogMenu : public Menu {
 public:
  BacklogMenu();

  virtual void Show();
  virtual void Hide();
  virtual void Update(float dt);
  virtual void UpdateInput(float dt);
  virtual void Render();

  void AddMessage(uint8_t* str, int audioId = -1);
  virtual void MenuButtonOnClick(Widgets::BacklogEntry* target);
  void Clear();

  float PageY = 0.0f;

 protected:
  int CurrentId = 0;
  float ItemsHeight = 0.0f;
  glm::vec2 CurrentEntryPos;
  Widgets::Group* MainItems;
  Animation FadeAnimation;
  Widgets::Scrollbar* MainScrollbar;

  float directionalButtonHeldTime = 0.0f;
  float advanceFocusWaitTime = 0.0f;

  void RenderHighlight() const;
  virtual Widgets::BacklogEntry* CreateBacklogEntry(
      int id, uint8_t* str, int audioId, glm::vec2 pos,
      const RectF& hoverBounds) const;
};

}  // namespace UI
}  // namespace Impacto