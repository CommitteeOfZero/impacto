#pragma once

#include "../../ui/menu.h"
#include "../../ui/savemenu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace MO8 {

class SaveMenu : public UI::SaveMenu {
 public:
  SaveMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  Widgets::Group* PageControls;
  Widgets::Group* MainItems;
  Animation FadeAnimation;
};

}  // namespace MO8
}  // namespace UI
}  // namespace Impacto