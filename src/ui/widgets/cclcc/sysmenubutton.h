#pragma once

#include "../button.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class SysMenuButton : public Widgets::Button {
 private:
 public:
  SysMenuButton(int id, Sprite const& norm, Sprite const& focused,
                Sprite const& highlight, glm::vec2 pos, RectF buttonBounds)
      : Widgets::Button(id, norm, focused, highlight, pos, buttonBounds),
        RenderPos(pos) {}
  void Render() override;
  glm::vec2 RenderPos;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto