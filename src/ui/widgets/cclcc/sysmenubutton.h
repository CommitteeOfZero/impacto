#pragma once

#include "../button.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class SysMenuButton final : public Widgets::Button {
 public:
  SysMenuButton(int id, Sprite const& norm, Sprite const& focused,
                Sprite const& highlight, glm::vec2 pos, RectF buttonBounds)
      : Widgets::Button(id, norm, focused, highlight, pos, buttonBounds),
        RenderPos(pos) {}

  void Render() override;

  void Move(glm::vec2 relativePosition) override {
    Button::Move(relativePosition);
    RenderPos += relativePosition;
  }

 private:
  glm::vec2 RenderPos;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto