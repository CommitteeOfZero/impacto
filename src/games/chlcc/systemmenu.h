#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class SystemMenu : public Menu {
 public:
  SystemMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  void DrawRunningSelectedLabel(float offsetY);
  void UpdateRunningSelectedLabel(float dt);
  float currentRunningPosition = 0.0f;
  int GetIndexOfActiveButton();
  void DrawCircles();
  void DrawErin();
  Widgets::Group* MainItems;
  Animation MenuTransition;
  Animation TitleFade;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto