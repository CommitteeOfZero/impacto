#include "sysmenubutton.h"

#include "../../../renderer2d.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace RNE {

void SysMenuButton::Render() {
  glm::vec4 black = glm::vec4(0.0f);
  black.a = Tint.a;

  if (HasFocus) {
    Renderer2D::DrawSprite(HighlightSprite, glm::vec2(Bounds.X, Bounds.Y),
                           Tint);
    Renderer2D::DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  } else {
    if (Enabled) {
      Renderer2D::DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y),
                             black);
    } else {
      Renderer2D::DrawSprite(DisabledSprite, glm::vec2(Bounds.X, Bounds.Y),
                             Tint);
    }
  }
}

}  // namespace RNE
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto