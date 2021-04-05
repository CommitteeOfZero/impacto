#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgetgroup.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace RNE {

class SystemMenu : public Menu {
 public:
  SystemMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  WidgetGroup* MainItems;
  Animation FadeAnimation;
};

}  // namespace RNE
}  // namespace UI
}  // namespace Impacto