#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/carousel.h"
#include "../../ui/widgets/label.h"

#include "../../profile/games/chlcc/trophymenu.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class TrophyMenu : public UI::Menu {
 public:
  TrophyMenu();

  void Show();
  void Hide();
  void UpdateInput(float dt);
  void Update(float dt);
  void Render();

 private:
  void DrawCircles();
  void DrawErin();
  void DrawRedBar();
  void DrawButtonPrompt();
  void UpdateTitles();

  Animation MenuTransition;
  Animation TitleFade;
  Animation FromSystemMenuTransition;

  glm::vec2 RedTitleLabelPos;
  glm::vec2 RightTitlePos;
  glm::vec2 LeftTitlePos;

  Impacto::UI::Widgets::Group
      MainItems[Impacto::Profile::CHLCC::TrophyMenu::MaxTrophyPages] = {
          this, this, this, this, this, this, this, this, this};
  int CurrentPage = 0;

  glm::vec2 Offset;
  Impacto::UI::Widgets::Label TrophyCountHintLabel;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto