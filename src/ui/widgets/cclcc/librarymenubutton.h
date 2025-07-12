#pragma once

#include <string>
#include <functional>

#include "../../../profile/games/cclcc/librarymenu.h"
#include "../../../animation.h"
#include "../button.h"
#include "../label.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class LibraryMenuButton : public Widgets::Button {
 public:
  LibraryMenuButton(int id, Sprite const& hovered, Sprite const& selected,
                    glm::vec2 pos, Animation& blinkAnimation)
      : Widgets::Button(
            id, Sprite(), selected, hovered, pos,
            RectF(pos.x, pos.y, hovered.Bounds.Width, hovered.Bounds.Height)),
        BlinkAnimation(blinkAnimation) {
    Bounds = HoverBounds;
  }
  void Render() override {
    if (!Enabled) {
      return;
    }
    if (Selected) {
      Renderer->DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
    } else if (HasFocus) {
      using namespace Profile::CCLCC::LibraryMenu;

      const glm::vec4 blinkMask{glm::vec3{1 - BlinkAnimation.Progress} *
                                        (1.0f - ButtonBlinkTintMinimum) +
                                    ButtonBlinkTintMinimum,
                                1};
      Renderer->DrawSprite(HighlightSprite, glm::vec2(Bounds.X, Bounds.Y),
                           Tint * blinkMask);
    }
  }
  bool Selected = false;
  Animation& BlinkAnimation;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
