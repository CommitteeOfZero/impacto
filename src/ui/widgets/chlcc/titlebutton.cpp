#include "titlebutton.h"

#include "../../../renderer/renderer.h"
#include "../../../profile/games/chlcc/titlemenu.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::TitleMenu;

void TitleButton::Render() {
  if (HasFocus) {
    if (!IsSubButton) {  // Main buttons
      Renderer->DrawSprite(HighlightSprite,
                           Bounds.GetPos() - ItemHighlightOffset, Tint);
      Renderer->DrawSprite(FocusedSprite, Bounds.GetPos(), Tint);
    } else {  // Sub buttons
      Renderer->DrawSprite(
          HighlightSprite,
          glm::vec2(SecondaryItemHighlightX, Bounds.Y - ItemHighlightOffset.y),
          Tint);
      Renderer->DrawSprite(FocusedSprite, Bounds.GetPos(), Tint);
      Renderer->DrawSprite(LineDecoration, glm::vec2(SecondaryMenuLineX, LineY),
                           Tint);
    }
  } else {
    if (Enabled) {
      Renderer->DrawSprite(NormalSprite, Bounds.GetPos(), Tint);
    } else {
      Renderer->DrawSprite(DisabledSprite, Bounds.GetPos(), Tint);
    }
  }
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto