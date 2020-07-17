#include "titlebutton.h"

#include "../../../renderer2d.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

void TitleButton::Render() {
  glm::vec4 black(0.0f);
  black.a = Opacity;
  glm::vec4 white(1.0f);
  white.a = Opacity;

  if (HasFocus) {
    Renderer2D::DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y + 1.0f),
                           black);
    Renderer2D::DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y), white);
  } else {
    if (Enabled) {
      Renderer2D::DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y + 1.0f),
                             black);
      Renderer2D::DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y),
                             white);
    } else {
      Renderer2D::DrawSprite(DisabledSprite,
                             glm::vec2(Bounds.X, Bounds.Y + 1.0f), black);
      Renderer2D::DrawSprite(DisabledSprite, glm::vec2(Bounds.X, Bounds.Y),
                             white);
    }
  }
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto