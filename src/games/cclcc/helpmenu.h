#pragma once

#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/label.h"
#include "../../animation.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class HelpMenu : public Widget {
 public:
  HelpMenu();
  void Show();
  void Hide();
  void Update(float dt) override;
  void Render() override;
  void UpdateInput() override;

 private:
  void NextImage();
  void PreviousImage();

  Widgets::Group* HelpItems;
  Widgets::Button* NextButton;
  Widgets::Button* PrevButton;
  std::vector<Sprite> HelpImages;
  int CurrentImageIndex = 0;
  Animation ImageTransitionAnimation;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto