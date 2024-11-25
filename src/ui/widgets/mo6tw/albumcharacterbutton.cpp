#include "albumcharacterbutton.h"

#include "../../../renderer/renderer.h"

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

  HighlightAnimation.Direction = AnimationDirection::In;
  HighlightAnimation.LoopMode = AnimationLoopMode::ReverseDirection;
  HighlightAnimation.SetDuration(highlightAnimationDuration);
  HighlightAnimation.StartIn();
}

void AlbumCharacterButton::Update(float dt) {
  Button::Update(dt);
  HighlightAnimation.Update(dt);
}

void AlbumCharacterButton::Render() {
  if (IsLocked) {
    Renderer->DrawSprite(LockedSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  } else {
    Renderer->DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  }

  glm::vec4 col = Tint;
  col.a = glm::smoothstep(0.0f, 1.0f, HighlightAnimation.Progress);
  if (HasFocus) {
    if (IsLocked) {
      Renderer->DrawSprite(LockedHighlightSprite, glm::vec2(Bounds.X, Bounds.Y),
                           col);
    } else {
      Renderer->DrawSprite(HighlightSprite, glm::vec2(Bounds.X, Bounds.Y), col);
    }
  }
}

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto