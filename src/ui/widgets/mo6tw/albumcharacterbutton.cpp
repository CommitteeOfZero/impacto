#include "albumcharacterbutton.h"

#include "../../../renderer2d.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

AlbumCharacterButton::AlbumCharacterButton(int id, Sprite const& norm,
                                           Sprite const& locked,
                                           Sprite const& highlight,
                                           Sprite const& lockedHighlight,
                                           glm::vec2 pos,
                                           float highlightAnimationDuration) {
  Id = id;
  NormalSprite = norm;
  HighlightSprite = highlight;
  LockedSprite = locked;
  LockedHighlightSprite = lockedHighlight;
  Enabled = true;
  Bounds = RectF(pos.x, pos.y, NormalSprite.ScaledWidth(),
                 NormalSprite.ScaledHeight());

  HighlightAnimation.Direction = 1;
  HighlightAnimation.LoopMode = ALM_ReverseDirection;
  HighlightAnimation.DurationIn = highlightAnimationDuration;
  HighlightAnimation.DurationOut = highlightAnimationDuration;
  HighlightAnimation.StartIn();
}

void AlbumCharacterButton::Update(float dt) {
  Button::Update(dt);
  HighlightAnimation.Update(dt);
}

void AlbumCharacterButton::Render() {
  if (IsLocked) {
    Renderer2D::DrawSprite(LockedSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  } else {
    Renderer2D::DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  }

  glm::vec4 col = Tint;
  col.a = glm::smoothstep(0.0f, 1.0f, HighlightAnimation.Progress);
  if (HasFocus) {
    if (IsLocked) {
      Renderer2D::DrawSprite(LockedHighlightSprite,
                             glm::vec2(Bounds.X, Bounds.Y), col);
    } else {
      Renderer2D::DrawSprite(HighlightSprite, glm::vec2(Bounds.X, Bounds.Y),
                             col);
    }
  }
}

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto