#pragma once

#include "../../ui/optionsmenu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/carousel.h"
#include "../../ui/widgets/label.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class OptionsMenu : public UI::OptionsMenu {
 public:
  OptionsMenu();

  void Update(float dt) override;
  void Render() override;

 private:
  void DrawCircles();
  void DrawErin();
  void DrawRedBar();
  void DrawButtonPrompt();

  void UpdateTitles();
  void UpdateVisibility() override;

  Animation TitleFade;
  Animation FromSystemMenuTransition;

  glm::vec2 RedTitleLabelPos;
  glm::vec2 RightTitlePos;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto