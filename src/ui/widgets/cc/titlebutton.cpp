#include "titlebutton.h"

#include "../../../renderer2d.h"
#include "../../../profile/games/cc/titlemenu.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CC {

using namespace Impacto::Profile::CC::TitleMenu;

void TitleButton::Render() {
  if (HasFocus) {
    if (!IsSubButton) {  // Main buttons
      Renderer2D::DrawSprite(HighlightSprite, glm::vec2(Bounds.X, Bounds.Y),
                             Tint);
      /*Renderer2D::DrawSprite(
          ItemHighlightPointerSprite,
          glm::vec2(Bounds.X - ItemHighlightPointerY, Bounds.Y), Tint);*/
      Renderer2D::DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y),
                             Tint);
    } else {  // Sub buttons
      Renderer2D::DrawSprite(HighlightSprite, glm::vec2(Bounds.X, Bounds.Y),
                             Tint);
      Renderer2D::DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y),
                             Tint);
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

}  // namespace CC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto