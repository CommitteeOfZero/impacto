#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/label.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class HelpMenu : public Menu {
 public:
  HelpMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;
  void Render() override;

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