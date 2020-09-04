#include "titlebutton.h"

#include "../../../renderer2d.h"
#include "../../../profile/games/chlcc/titlemenu.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::TitleMenu;

void TitleButton::Render() {
  glm::vec4 black(0.0f);
  black.a = Opacity;
  glm::vec4 white(1.0f);
  white.a = Opacity;

  if (HasFocus) {
    if (!isSubButton) {  //Main buttons
    Renderer2D::DrawSprite(HighlightSprite,
                           glm::vec2(Bounds.X - ItemHighlightOffsetX,
                                     Bounds.Y - ItemHighlightOffsetY),
                           white);
    Renderer2D::DrawSprite(FocusedSprite,
                           glm::vec2(Bounds.X, Bounds.Y),
                           white);
    } else {  //Sub buttons
      Renderer2D::DrawSprite(
          HighlightSprite,
          glm::vec2(SecondaryItemHighlightX, Bounds.Y - ItemHighlightOffsetY),
          white);
      Renderer2D::DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y),
                             white);
      Renderer2D::DrawSprite(LineDecoration, glm::vec2(SecondaryMenuLineX, LineY),
                             white);
    }
  } else {
    if (Enabled) {
      Renderer2D::DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y),
                             white);
    } else {
      Renderer2D::DrawSprite(DisabledSprite, glm::vec2(Bounds.X, Bounds.Y),
                             white);
    }
  }
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto