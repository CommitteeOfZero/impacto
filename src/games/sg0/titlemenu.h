#pragma once

#include "../../animation.h"
#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace SG0 {

// STEINS;GATE 0 title menu. The script drives it through the same title
// protocol as CHAOS;CHILD (SW_TITLEMODE, SW_TITLEDISPCT, SF_TITLEEND) and reads
// the chosen item from SW_TITLECUR1: 0 NEW GAME, 10 LOAD, 11 QUICK LOAD,
// 20-24 CLEAR LIST / CG LIBRARY / SOUND LIBRARY / MOVIE LIBRARY / TIPS,
// 30 CONFIG, 40 HELP. The background is the intro movie the script plays.
class TitleMenu : public Menu {
 public:
  TitleMenu();
  void Show() override;
  void Hide() override;
  void UpdateInput(float dt) override;
  void Update(float dt) override;
  void Render() override;

  Animation PressToStartAnimation;

 private:
  void MenuButtonOnClick(Widgets::Button* target);
  void OpenSubMenu(Widgets::Group* subMenu, Widgets::Button* parent);
  void DrawLogo(float opacity);
  void DrawStartButton();

  Widgets::Group* MainItems;
  Widgets::Group* ContinueItems;
  Widgets::Group* ExtraItems;
  Widgets::Group* CurrentSubMenu = nullptr;
  Widgets::Button* NewGame;
};

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
