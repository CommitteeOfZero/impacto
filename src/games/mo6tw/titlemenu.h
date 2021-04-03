#pragma once

#include "../../animation.h"
#include "../../ui/menu.h"
#include "../../ui/widgetgroup.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/mo6tw/titlebutton.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

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

  void MenuButtonOnClick(Widgets::Button* target);
  void SecondaryButtonOnClick(Widgets::Button* target);

 private:
  WidgetGroup* MainItems;
  Widgets::MO6TW::TitleButton* InitialStart;
  Widgets::MO6TW::TitleButton* ExtraStory;
  Widgets::MO6TW::TitleButton* Continue;
  Widgets::MO6TW::TitleButton* Memories;
  Widgets::MO6TW::TitleButton* Encyclopedia;
  Widgets::MO6TW::TitleButton* System;
  Widgets::MO6TW::TitleButton* Exit;

  WidgetGroup* ExtraStoryItems;

  WidgetGroup* ContinueItems;
  Widgets::MO6TW::TitleButton* Load;
  Widgets::MO6TW::TitleButton* QuickLoad;

  WidgetGroup* MemoriesItems;

  WidgetGroup* SystemItems;
  Widgets::MO6TW::TitleButton* Option;
  Widgets::MO6TW::TitleButton* SystemSave;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto