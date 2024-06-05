#pragma once

#include "../button.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class SysMenuButton : public Widgets::Button {
 public:
  SysMenuButton(int id, Sprite const& norm, Sprite const& focused,
                Sprite const& highlight, glm::vec2 pos)
      : Widgets::Button(id, norm, focused, highlight, pos) {}
  void Render() override;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto