#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

class SystemMenu : public Menu {
 public:
  SystemMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void Render() override;

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  Widgets::Group* MainItems;
  Animation FadeAnimation;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto