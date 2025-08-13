#pragma once

#include "../../ui/widgets/button.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/label.h"
#include <vector>
#include <memory>

namespace Impacto {
namespace UI {
namespace CCLCC {
class LibrarySubmenu : public Menu {
 public:
  LibrarySubmenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  virtual void UpdateInput(float dt) override { Menu::UpdateInput(dt); };
  void Render() override;
  // void Move(glm::vec2 offset);
  // void MoveTo(glm::vec2 pos);
  virtual void Unfocus();
  Animation FadeAnimation;
  Widgets::Group MainItems{this};
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto