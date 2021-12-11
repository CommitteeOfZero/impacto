#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

class AlbumMenu : public Menu {
 public:
  AlbumMenu();

  void Show();
  void Hide();
  void UpdateInput();
  void Update(float dt);
  void Render();

 private:
  Widgets::Group* MainItems;
  Animation FadeAnimation;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto