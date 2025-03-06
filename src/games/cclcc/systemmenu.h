#pragma once

#include <glm/fwd.hpp>
#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../profile/games/cclcc/systemmenu.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class SystemMenu : public Menu {
 public:
  using GridVertices =
      std::array<glm::vec2, (Profile::CCLCC::SystemMenu::GridRowCount + 1) *
                                (Profile::CCLCC::SystemMenu::GridColCount + 1)>;

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

  GridVertices SpriteGridVertices;
  GridVertices DisplayGridVertices;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto