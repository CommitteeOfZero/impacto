#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/carousel.h"
#include "../../ui/widgets/label.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

class ClearListMenu : public Menu {
 public:
  ClearListMenu();

  void Show() override;
  void Hide() override;
  void UpdateInput(float dt) override;
  void Update(float dt) override;
  void Render() override;

  void OnAdvancePage(Widget* currentPage, Widget* nextPage);
  void OnGoBackPage(Widget* currentPage, Widget* nextPage);
  void ArrowLeftOnClick(Widgets::Button* target);
  void ArrowRightOnClick(Widgets::Button* target);

 private:
  void InitMainPage();
  void UpdatePlayTime();
  void UpdateEndingCount();
  void UpdateSceneCount();
  void UpdateAlbumCount();
  void UpdateCompletionPercentage();
  void InitEndingListPage();
  void UpdateEndingList();
  void InitSceneTitlePage();
  void UpdateSceneList();

  bool IsInit = false;

  Widgets::Carousel* MainItems;
  Widgets::Group* Arrows;
  Widgets::Group* MainPage;
  Widgets::Group* SceneTitlePage;
  Widgets::Group* EndingListPage;

  Widget* CurrentPage = 0;
  Widget* PreviousPage = 0;

  float SeparatorWidth;
  Widgets::Label* UnlockedEndingCount;
  float EndingCountWidth;
  Widgets::Label* UnlockedSceneCount;
  float SceneCountWidth;
  Widgets::Label* UnlockedAlbumCount;
  float AlbumCountWidth;
  Widgets::Label* CompletionPercentage;
  Widgets::Label* PlaySeconds;
  Widgets::Label* PlayMinutes;
  Widgets::Label* PlayHours;
  Widgets::Label* HoursText;
  float SecondsTextWidth;
  float MinutesTextWidth;
  float HoursTextWidth;

  Widget** EndingNames;
  Widget** SceneNames;

  float SceneListY;
  Widgets::Group* SceneTitleItems;

  Animation FadeAnimation;
  Animation ArrowsAnimation;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto