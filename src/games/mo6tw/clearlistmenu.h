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
  void UpdateInput();
  void Update(float dt);
  void Render();

  void AdvancePage(Widget* currentPage, Widget* nextPage);
  void GoBackPage(Widget* currentPage, Widget* nextPage);

 private:
  void InitMainPage();
  void UpdatePlayTime();
  void InitEndingListPage();
  void UpdateEndingList();
  void InitSceneTitlePage();

  bool IsInit = false;

  Widgets::Carousel* MainItems;
  Widgets::Group* MainPage;
  Widgets::Group* SceneTitlePage;
  Widgets::Group* EndingListPage;

  Widget* CurrentPage = 0;
  Widget* PreviousPage = 0;

  Widgets::Label* PlaySeconds;
  Widgets::Label* PlayMinutes;
  Widgets::Label* PlayHours;
  Widgets::Label* HoursText;
  float SecondsTextWidth;
  float MinutesTextWidth;
  float HoursTextWidth;

  Widget** EndingNames;

  Animation FadeAnimation;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto