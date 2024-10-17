#pragma once

#include "../../ui/menu.h"
#include "../../spriteanimation.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class OptionsMenu : public Menu {
 public:
  OptionsMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

 private:
  Animation FadeAnimation;
  SpriteAnimation PoleAnimation;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto