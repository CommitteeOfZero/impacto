#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/savemenu.h"
#include "../../profile/games/cclcc/savemenu.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class SaveMenu : public UI::SaveMenu {
 public:
  SaveMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void UpdateInput() override;
  void Render();

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  int PrevPage = 0;
  int CurrentPage = 0;
  Widgets::Group* MainItems[Profile::CCLCC::SaveMenu::Pages]{};
  Animation FadeAnimation;
  Animation PageAnimation;
  bool HasCleared = true;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto