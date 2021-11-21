#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace CC {

class TitleMenu : public Menu {
 public:
  TitleMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  Animation PressToStartAnimation;
  Animation SmokeAnimation;
  Animation MoveLeftAnimation;

  void MenuButtonOnClick(Widgets::Button* target);
  void SecondaryButtonOnClick(Widgets::Button* target);

 private:
  void DrawMainBackground(float opacity = 1.0f);
  void DrawStartButton();
  void DrawSmoke(float opacity);
};

}  // namespace CC
}  // namespace UI
}  // namespace Impacto