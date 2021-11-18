#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgetgroup.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/scrollbar.h"
#include "../../ui/widgets/toggle.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

class OptionsMenu : public Menu {
 public:
  OptionsMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  WidgetGroup* MainItems;
  Animation FadeAnimation;
  Widgets::Toggle* TestToggle;
  Widgets::Scrollbar* MainScrollbar;
  bool VoiceTest[14];
  float Test = 0.0f;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto