#pragma once

#include "../../animation.h"
#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/cc/titlebutton.h"

namespace Impacto {
namespace UI {
namespace CC {

class TitleMenu : public Menu {
 public:
  TitleMenu();

  void Show();
  void Hide();
  void UpdateInput(float dt);
  void Update(float dt);
  void Render();

  Animation PressToStartAnimation;
  Animation ItemsFadeInAnimation;
  Animation SecondaryItemsFadeInAnimation;
  Animation SmokeAnimation;
  Animation MoveLeftAnimation;

  void MenuButtonOnClick(Widgets::Button* target);
  void ContinueButtonOnClick(Widgets::Button* target);
  void ExtraButtonOnClick(Widgets::Button* target);

 private:
  Widgets::Group* CurrentSubMenu = 0;

  Widgets::Group* MainItems;
  Widgets::CC::TitleButton* NewGame;
  Widgets::CC::TitleButton* Continue;
  Widgets::CC::TitleButton* Extra;
  Widgets::CC::TitleButton* Config;
  Widgets::CC::TitleButton* Help;

  Widgets::Group* ContinueItems;
  Widgets::CC::TitleButton* Load;
  Widgets::CC::TitleButton* QuickLoad;
  // void ShowContinueItems();
  // void HideContinueItems();

  Widgets::Group* ExtraItems;
  Widgets::CC::TitleButton* Tips;
  Widgets::CC::TitleButton* Library;
  Widgets::CC::TitleButton* EndingList;
  // void ShowExtraItems();
  // void HideExtraItems();

  void DrawMainBackground(float opacity = 1.0f);
  void DrawStartButton();
  void DrawSmoke(float opacity);
};

}  // namespace CC
}  // namespace UI
}  // namespace Impacto