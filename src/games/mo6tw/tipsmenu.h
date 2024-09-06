#pragma once

#include "../../ui/tipsmenu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/carousel.h"
#include "../../ui/widgets/label.h"
#include "../../data/tipssystem.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

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
  int CurrentTipPage = 1;

  Widgets::Carousel ItemsList;
  Widgets::Group TipViewItems;
  Sprite* ThumbnailSprite;
  Widgets::Label* PageSeparator;
  Widgets::Label* CurrentPage;
  Widgets::Label* TotalPages;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto