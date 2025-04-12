#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/label.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class HelpMenu : public Menu {
 public:
  HelpMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void UpdateInput(float dt);
  void Render();

  Animation FadeAnimation;
  Animation NextPageAnimation;

 private:
  int CurrentPage = 0;
  int PreviousPage = -1;
  bool IsGoingNext = true;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto