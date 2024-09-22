#include "titlebutton.h"

#include "../../../renderer/renderer.h"
#include "../../../profile/games/cclcc/titlemenu.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

using namespace Impacto::Profile::CCLCC::TitleMenu;

void TitleButton::UpdateInput() {
  if (IsSubButton || HighlightAnimation.State == AS_Stopped) {
    Button::UpdateInput();
  }
}

void TitleButton::Render() {
  if (HasFocus || HighlightAnimation.State == AS_Playing) {
    if (!IsSubButton) {  // Main buttons
      Sprite newHighlightSprite = HighlightSprite;
      float smoothProgress =
          HighlightAnimation.State == AS_Playing
              ? glm::smoothstep(0.0f, 1.0f, HighlightAnimation.Progress)
              : 1.0f;
      newHighlightSprite.Bounds.Width *= smoothProgress;
      Renderer->DrawSprite(newHighlightSprite,
                           glm::vec2(Bounds.X - ItemHighlightOffsetX,
                                     Bounds.Y - ItemHighlightOffsetY),
                           Tint);
      glm::vec4 pointerTint = glm::vec4(1.0f, 1.0f, 1.0f, smoothProgress);
      Renderer->DrawSprite(
          ItemHighlightPointerSprite,
          glm::vec2(Bounds.X - ItemHighlightPointerY, Bounds.Y), pointerTint);
      Renderer->DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
    } else {  // Sub buttons
      Renderer->DrawSprite(HighlightSprite, glm::vec2(Bounds.X, Bounds.Y),
                           Tint);
      Renderer->DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
    }
  } else {
    if (Enabled) {
      Renderer->DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
    } else {
      Renderer->DrawSprite(DisabledSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
    }
  }
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto