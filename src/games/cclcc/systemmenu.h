#pragma once

#include <glm/fwd.hpp>
#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class SystemMenu : public Menu {
 public:
  SystemMenu();

  void InitPosition();
  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  Widgets::Group* MainItems;
  Animation MenuTransition;
  Animation MenuFade;
  Animation ItemsFade;
  bool ItemsFadeComplete = false;
  Sprite ScreenCap;
  glm::vec2 BGPosition{};
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto