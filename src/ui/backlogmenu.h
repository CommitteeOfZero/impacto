#pragma once

#include "menu.h"
#include "widgetgroup.h"
#include "widgets/backlogentry.h"
#include "widgets/scrollbar.h"

namespace Impacto {
namespace UI {

class BacklogMenu : public Menu {
 public:
  BacklogMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void AddMessage(uint8_t* str, int audioId = -1);
  void MenuButtonOnClick(Widgets::BacklogEntry* target);
  void Clear();

  float PageY = 0.0f;

 private:
  int CurrentId = 0;
  float ItemsHeight = 0.0f;
  glm::vec2 CurrentEntryPos;
  WidgetGroup* MainItems;
  Animation FadeAnimation;
  Widgets::Scrollbar* MainScrollbar;
};

}  // namespace UI
}  // namespace Impacto