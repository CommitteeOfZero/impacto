#pragma once

#include "../../animation.h"
#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace SG0 {

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
