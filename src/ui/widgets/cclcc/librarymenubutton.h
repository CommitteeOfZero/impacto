#pragma once

#include <string>
#include <functional>

#include "../button.h"
#include "../label.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class LibraryMenuButton : public Widgets::Button {
 public:
  LibraryMenuButton(int id, Sprite const& hovered, Sprite const& selected,
                    glm::vec2 pos)
      : Widgets::Button(
            id, Sprite(), selected, hovered, pos,
            RectF(pos.x, pos.y, hovered.Bounds.Width, hovered.Bounds.Height)) {
    Bounds = HoverBounds;
  }
  void Render() override {
    if (!Enabled) {
      return;
    }
    if (Selected) {
      Renderer->DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
    } else if (HasFocus) {
      Renderer->DrawSprite(HighlightSprite, glm::vec2(Bounds.X, Bounds.Y),
                           Tint);
    }
  }
  bool Selected = false;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
