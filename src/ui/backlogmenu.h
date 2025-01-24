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

  virtual void Show();
  virtual void Hide();
  virtual void Update(float dt);
  virtual void UpdateInput(float dt);
  virtual void Render();

  void AddMessage(uint8_t* str, int audioId = -1, int characterId = 0);
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

  TurboOnHoldHandler DirectionButtonHeldHandler;
  TurboOnHoldHandler PageUpDownButtonHeldHandler;

  void RenderHighlight() const;

  void UpdatePageUpDownInput(float dt);
  void UpdateScrollingInput(float dt);
};

}  // namespace UI
}  // namespace Impacto