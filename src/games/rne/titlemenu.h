#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace RNE {

class TitleMenu : public Menu {
 public:
  TitleMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();
};

}  // namespace RNE
}  // namespace UI
}  // namespace Impacto