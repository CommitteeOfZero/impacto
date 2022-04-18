#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class SaveMenu : public Menu {
 public:
  SaveMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  Widgets::Group* MainItems;
  Animation FadeAnimation;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto