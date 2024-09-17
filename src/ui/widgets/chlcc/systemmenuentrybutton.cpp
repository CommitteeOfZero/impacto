#include "systemmenuentrybutton.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

SystemMenuEntryButton::SystemMenuEntryButton(int id, Sprite const& norm,
                                             Sprite const& focused,
                                             glm::vec4 focusTint,
                                             Sprite const& highlight,
                                             glm::vec2 pos)
    : Button(id, norm, focused, highlight, pos), FocusTint(focusTint) {
  HighlightOffset = glm::vec2(3.0f, 3.0f);
}

void SystemMenuEntryButton::Render() {
  if (HasFocus) {
    Renderer->DrawSprite(
        NormalSprite,
        glm::vec2(Bounds.X + HighlightOffset.x, Bounds.Y + HighlightOffset.y),
        RgbIntToFloat(0x888888));
  }
  Renderer->DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y),
                       HasFocus ? FocusTint : glm::vec4(1.0f));
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto