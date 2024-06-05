#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class SystemMenu : public Menu {
 public:
  SystemMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  inline void DrawCircles();
  inline void DrawErin();
  Widgets::Group* MainItems;
  Animation MenuTransition;
  Animation TitleFade;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto