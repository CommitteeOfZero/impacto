#pragma once

#include "menu.h"
#include "widgetgroup.h"
#include "widgets/backlogentry.h"

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

 private:
  int CurrentId = 0;
  glm::vec2 CurrentEntryPos;
  WidgetGroup* MainItems;
  Animation FadeAnimation;
};

}  // namespace UI
}  // namespace Impacto