#pragma once

#include "../../animation.h"
#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/sghd/titlebutton.h"

using Impacto::UI::Widgets::SGHD::TitleButton;

namespace Impacto {
namespace UI {
namespace SGHD {

class TitleMenu : public Menu {
 public:
  TitleMenu();

  void Show();
  void Hide();
  void UpdateInput();
  void Update(float dt);
  void Render();

 private:
  Animation PressToStartAnimation;

  Widgets::Group* MainItems;
  TitleButton* Start;
  TitleButton* Load;
  TitleButton* Extra;
  TitleButton* Config;
  TitleButton* Help;

  void DrawMainBackground();
  void DrawStartButton();

  void MenuButtonOnClick(Widgets::Button* target);
  void DrawMainBackground(float opacity = 1.0f);
};

}  // namespace SGHD
}  // namespace UI
}  // namespace Impacto
