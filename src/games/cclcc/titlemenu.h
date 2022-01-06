#pragma once

#include "../../animation.h"
#include "../../ui/menu.h"
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
  void Update(float dt);
  void Render();

  Animation PressToStartAnimation;
  Animation PrimaryFadeAnimation;
  Animation SecondaryFadeAnimation;
  Animation ItemsFadeInAnimation;
  Animation SecondaryItemsFadeInAnimation;
  Animation SpinningCircleAnimation;

  void MenuButtonOnClick(Widgets::Button* target);
  void SecondaryButtonOnClick(Widgets::Button* target);

  void DrawTitleMenuBackGraphics();
  void DrawMainMenuBackGraphics();

 private:
  Widgets::Group* MainItems;
  Widgets::CCLCC::TitleButton* NewGame;
  Widgets::CCLCC::TitleButton* Continue;
  Widgets::CCLCC::TitleButton* Extra;
  Widgets::CCLCC::TitleButton* Config;
  Widgets::CCLCC::TitleButton* Help;

  Widgets::Group* ContinueItems;
  Widgets::CCLCC::TitleButton* Load;
  Widgets::CCLCC::TitleButton* QuickLoad;

  Widgets::Group* ExtraItems;
  Widgets::CCLCC::TitleButton* Tips;
  Widgets::CCLCC::TitleButton* Library;
  Widgets::CCLCC::TitleButton* EndingList;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto