#pragma once

#include "../../ui/tipsmenu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/carousel.h"
#include "../../ui/widgets/label.h"
#include "../../data/tipssystem.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class TipsMenu : public UI::TipsMenu {
 public:
  TipsMenu();

  void Init();

  void Show();
  void Hide();
  void UpdateInput();
  void Update(float dt);
  void Render();

  void TipOnClick(Widgets::Button* target);

 protected:
  void SwitchToTipId(int id);
  void NextTipPage();

 private:
  /*Impacto::TipsSystem::TipsDataRecord* Records;

  int CurrentTipPage = 1;
  int CurrentPageIdx = 0;
  int PageCount = 0;

  Sprite* ThumbnailSprite;
  Widgets::Carousel* ItemsList;
  Widgets::Group* TipViewItems;
  Widgets::Label* PageSeparator;
  Widgets::Label* CurrentPage;
  Widgets::Label* TotalPages;*/
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto