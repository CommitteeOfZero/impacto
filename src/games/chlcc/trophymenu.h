#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/carousel.h"
#include "../../ui/widgets/label.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class TrophyMenu : public UI::Menu {
 public:
  TrophyMenu();

  void Init();

  void Show();
  void Hide();
  void UpdateInput();
  void Update(float dt);
  void Render();

 private:
  inline void DrawCircles();
  inline void DrawErin();
  inline void DrawRedBar();
  inline void DrawButtonPrompt();
  void UpdateTitles();

  Animation MenuTransition;
  Animation TitleFade;

  glm::vec2 RedTitleLabelPos;
  glm::vec2 RightTitlePos;
  glm::vec2 LeftTitlePos;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto