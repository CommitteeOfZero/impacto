#pragma once

#include "../button.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace RNE {

class SysMenuButton : public Widgets::Button {
 public:
  SysMenuButton(int id, Sprite norm, Sprite highlight, glm::vec2 pos)
      : Widgets::Button(id, norm, std::nullopt, highlight, pos) {}
  void Render() override;
};

}  // namespace RNE
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto