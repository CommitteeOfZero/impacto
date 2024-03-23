#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/group.h"

namespace Impacto {
namespace UI {
namespace MO8 {

class TitleMenu : public Menu {
 public:
  TitleMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  Widgets::Group* MainItems;

  Widgets::Button* NewGame;
  Widgets::Button* Continue;
  Widgets::Button* Options;
};

}  // namespace MO8
}  // namespace UI
}  // namespace Impacto