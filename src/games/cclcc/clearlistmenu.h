#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/label.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class ClearListMenu : public Menu {
 public:
  ClearListMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void Render() override;

  void DrawEndingSprites(const glm::vec4& transition);

  Animation FadeAnimation;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto