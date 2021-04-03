#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgetgroup.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace Dash {

class TitleMenu : public Menu {
 public:
  TitleMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();
};

}  // namespace Dash
}  // namespace UI
}  // namespace Impacto