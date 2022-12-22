#include "actorsvoicebutton.h"

#include "../../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

ActorsVoiceButton::ActorsVoiceButton(int id, Sprite const& norm,
                                     Sprite const& locked,
                                     Sprite const& highlight,
                                     Sprite const& lockedHighlight,
                                     glm::vec2 pos) {
  Id = id;
  NormalSprite = norm;
  HighlightSprite = highlight;
  LockedSprite = locked;
  LockedHighlightSprite = lockedHighlight;
  Enabled = true;
  Bounds = RectF(pos.x, pos.y, NormalSprite.ScaledWidth(),
                 NormalSprite.ScaledHeight());
}

void ActorsVoiceButton::Render() {
  if (IsLocked) {
    Renderer->DrawSprite(LockedSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  } else {
    Renderer->DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  }
  if (HasFocus) {
    if (IsLocked) {
      Renderer->DrawSprite(LockedHighlightSprite,
                             glm::vec2(Bounds.X, Bounds.Y), Tint);

    } else {
      Renderer->DrawSprite(HighlightSprite, glm::vec2(Bounds.X, Bounds.Y),
                             Tint);
    }
  }
}

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto