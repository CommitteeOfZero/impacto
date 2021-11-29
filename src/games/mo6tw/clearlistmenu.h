#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/carousel.h"
#include "../../ui/widgets/label.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

class ClearListMenu : public Menu {
 public:
  ClearListMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

 private:
  void InitMainPage();
  void UpdatePlayTime();
  void InitEndingListPage();
  void InitSceneTitlePage();

  bool IsInit = false;

  Widgets::Carousel* MainItems;
  Widgets::Group* MainPage;
  Widgets::Group* SceneTitlePage;
  Widgets::Group* EndingListPage;

  Widgets::Label* PlaySeconds;
  Widgets::Label* PlayMinutes;
  Widgets::Label* PlayHours;
  Widgets::Label* HoursText;
  float SecondsTextWidth;
  float MinutesTextWidth;
  float HoursTextWidth;

  Animation FadeAnimation;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto