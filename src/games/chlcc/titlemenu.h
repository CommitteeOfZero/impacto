#pragma once

#include "../../animation.h"
#include "../../ui/menu.h"
#include "../../ui/widgetgroup.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/chlcc/titlebutton.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

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

 private:
  WidgetGroup* MainItems;
  Widgets::CHLCC::TitleButton* Start;
  Widgets::CHLCC::TitleButton* Load;
  Widgets::CHLCC::TitleButton* Extra;
  Widgets::CHLCC::TitleButton* System;

  WidgetGroup* LoadItems;
  Widgets::CHLCC::TitleButton* SubLoad;
  Widgets::CHLCC::TitleButton* QuickLoad;

  WidgetGroup* ExtraItems;
  Widgets::CHLCC::TitleButton* ClearList;
  Widgets::CHLCC::TitleButton* CGLibrary;
  Widgets::CHLCC::TitleButton* SoundLibrary;
  Widgets::CHLCC::TitleButton* MovieLibrary;
  Widgets::CHLCC::TitleButton* Tips;
  Widgets::CHLCC::TitleButton* Trophy;

  WidgetGroup* SystemItems;
  Widgets::CHLCC::TitleButton* Config;
  Widgets::CHLCC::TitleButton* SystemSave;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto