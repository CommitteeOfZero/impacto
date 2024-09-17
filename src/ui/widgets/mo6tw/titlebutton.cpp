#include "titlebutton.h"

#include "../../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

void TitleButton::Render() {
  glm::vec4 black(0.0f);
  black.a = Tint.a;
  glm::vec4 white(1.0f);
  white.a = Tint.a;

  if (HasFocus && Enabled) {
    Renderer->DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y + 1.0f),
                         black);
    Renderer->DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y), white);
  } else if (Enabled) {
    Renderer->DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y + 1.0f),
                         black);
    Renderer->DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y), white);
  } else {
    Renderer->DrawSprite(DisabledSprite, glm::vec2(Bounds.X, Bounds.Y + 1.0f),
                         black);
    Renderer->DrawSprite(DisabledSprite, glm::vec2(Bounds.X, Bounds.Y), white);
  }
}

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto