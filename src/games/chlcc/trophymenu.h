#pragma once

#include "commonmenu.h"
#include "animations/menutransition.h"
#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/carousel.h"
#include "../../ui/widgets/label.h"

#include "../../profile/games/chlcc/trophymenu.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class TrophyMenu : public Menu, public CommonMenu {
 public:
  TrophyMenu();

  void Show();
  void Hide();
  void UpdateInput(float dt);
  void Update(float dt);
  void Render();

 private:
  Impacto::UI::Widgets::Group
      MainItems[Impacto::Profile::CHLCC::TrophyMenu::MaxTrophyPages] = {
          this, this, this, this, this, this, this, this, this};
  int CurrentPage = 0;

  Impacto::UI::Widgets::Label TrophyCountHintLabel;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto