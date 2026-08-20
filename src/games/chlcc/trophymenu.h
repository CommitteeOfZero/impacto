#pragma once

#include "commonmenu.h"
#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
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
  Widgets::Group MainItems[Profile::CHLCC::TrophyMenu::MaxTrophyPages] = {
      this, this, this, this, this, this, this, this, this};
  int CurrentPage = 0;

  Widgets::Label TrophyCountHintLabel;

  Widgets::Label PlatinumCountLabel;
  Widgets::Label GoldCountLabel;
  Widgets::Label SilverCountLabel;
  Widgets::Label BronzeCountLabel;

  void UpdateRarityCounts();
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto