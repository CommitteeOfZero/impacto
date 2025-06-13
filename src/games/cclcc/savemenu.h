#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../profile/games/cclcc/savemenu.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class SaveMenu : public Menu {
 public:
  SaveMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void UpdateInput() override;
  void Render() override;

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  int CurrentPage = 0;
  Widgets::Group* MainItems[Profile::CCLCC::SaveMenu::Pages]{};
  Animation FadeAnimation;
  bool HasCleared = true;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto