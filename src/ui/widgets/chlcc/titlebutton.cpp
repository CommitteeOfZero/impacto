#include "titlebutton.h"

#include "../../../renderer2d.h"
#include "../../../profile/games/chlcc/titlemenu.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::TitleMenu;

void TitleButton::Render() {
  if (HasFocus) {
    if (!IsSubButton) {  // Main buttons
      Renderer2D::DrawSprite(HighlightSprite,
                             glm::vec2(Bounds.X - ItemHighlightOffsetX,
                                       Bounds.Y - ItemHighlightOffsetY),
                             Tint);
      Renderer2D::DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y),
                             Tint);
    } else {  // Sub buttons
      Renderer2D::DrawSprite(
          HighlightSprite,
          glm::vec2(SecondaryItemHighlightX, Bounds.Y - ItemHighlightOffsetY),
          Tint);
      Renderer2D::DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y),
                             Tint);
      Renderer2D::DrawSprite(LineDecoration,
                             glm::vec2(SecondaryMenuLineX, LineY), Tint);
    }
  } else {
    if (Enabled) {
      Renderer2D::DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
    } else {
      Renderer2D::DrawSprite(DisabledSprite, glm::vec2(Bounds.X, Bounds.Y),
                             Tint);
    }
  }
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto