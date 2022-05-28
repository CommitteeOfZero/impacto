#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/scrollbar.h"
#include "../../ui/widgets/toggle.h"

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
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto