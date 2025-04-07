#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/label.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class ClearListMenu : public Menu {
 public:
  ClearListMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void DrawEndingSprites(const glm::vec4& transition);

  Animation FadeAnimation;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto