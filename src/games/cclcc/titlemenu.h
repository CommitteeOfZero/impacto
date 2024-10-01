#pragma once

#include "../../animation.h"
#include "../../character2d.h"
#include "../../ui/menu.h"
#include "../../ui/widgets/label.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/cclcc/titlebutton.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class TitleMenu : public Menu {
 public:
  TitleMenu();

  void Show();
  void Hide();
  void UpdateInput();
  void Update(float dt);
  void Render();

  Animation PressToStartAnimation;
  Animation PrimaryFadeAnimation;
  Animation SecondaryFadeAnimation;
  Animation SmokeAnimation;
  Animation TitleAnimation;
  Animation SlideItemsAnimation;
  Character2D TitleAnimationSprite;

  void MenuButtonOnClick(Widgets::Button* target);
  void ContinueButtonOnClick(Widgets::Button* target);
  void ExtraButtonOnClick(Widgets::Button* target);

 private:
  Widgets::Group* CurrentSubMenu = 0;

  Widgets::Group* MainItems;
  Widgets::CCLCC::TitleButton* NewGame;
  Widgets::CCLCC::TitleButton* Continue;
  Widgets::CCLCC::TitleButton* Extra;
  Widgets::CCLCC::TitleButton* Config;
  Widgets::CCLCC::TitleButton* Help;
  Widgets::Label* MenuLabel;

  Widgets::Group* ContinueItems;
  Widgets::CCLCC::TitleButton* Load;
  Widgets::CCLCC::TitleButton* QuickLoad;

  void ShowContinueItems();
  void HideContinueItems();

  Widgets::Group* ExtraItems;
  Widgets::CCLCC::TitleButton* Tips;
  Widgets::CCLCC::TitleButton* Library;
  Widgets::CCLCC::TitleButton* EndingList;
  void ShowExtraItems();
  void HideExtraItems();

  void DrawDISwordBackground(float opacity = 1.0f);
  void DrawStartButton();
  void DrawMainMenuBackGraphics();
  void DrawSmoke(float opacity);

  void MainMenuUpdate();
  void SubMenuUpdate();
  bool DisableInputReset = false;
  MenuState SubMenuState = Hidden;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto