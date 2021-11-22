#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

class TipsMenu : public Menu {
 public:
  TipsMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

 private:
  Animation FadeAnimation;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto