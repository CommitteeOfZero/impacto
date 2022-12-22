#include "imagethumbnailbutton.h"

#include "../../../renderer/renderer.h"
#include "../../../profile/games/mo6tw/moviemenu.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

using namespace Impacto::Profile::MO6TW::MovieMenu;

ImageThumbnailButton::ImageThumbnailButton(int id, Sprite const& norm,
                                           Sprite const& disabled,
                                           Sprite const& focusedTopLeft,
                                           Sprite const& focusedTopRight,
                                           Sprite const& focusedBottomLeft,
                                           Sprite const& focusedBottomRight,
                                           glm::vec2 pos) {
  Id = id;
  NormalSprite = norm;
  DisabledSprite = disabled;
  HighlightTopLeft = focusedTopLeft;
  HighlightTopRight = focusedTopRight;
  HighlightBottomLeft = focusedBottomLeft;
  HighlightBottomRight = focusedBottomRight;
  Enabled = true;
  Bounds = RectF(pos.x, pos.y, NormalSprite.ScaledWidth(),
                 NormalSprite.ScaledHeight());

  HighlightAnimation.Direction = 1;
  HighlightAnimation.LoopMode = ALM_Loop;
  HighlightAnimation.DurationIn = HighlightAnimationDuration;
  HighlightAnimation.DurationOut = HighlightAnimationDuration;
  HighlightAnimation.StartIn();
}

ImageThumbnailButton::ImageThumbnailButton(
    int id, Sprite const& normTopPart, Sprite const& normBottomPart,
    Sprite const& disabled, Sprite const& focusedTopLeft,
    Sprite const& focusedTopRight, Sprite const& focusedBottomLeft,
    Sprite const& focusedBottomRight, glm::vec2 pos)
    : ImageThumbnailButton(id, normTopPart, disabled, focusedTopLeft,
                           focusedTopRight, focusedBottomLeft,
                           focusedBottomRight, pos) {
  IsSplit = true;
  BottomPart = normBottomPart;
  Bounds = RectF(pos.x, pos.y, NormalSprite.ScaledWidth(),
                 NormalSprite.ScaledHeight() + BottomPart.ScaledHeight());
}

void ImageThumbnailButton::Update(float dt) {
  Button::Update(dt);
  HighlightAnimation.Update(dt);
}

void ImageThumbnailButton::Render() {
  if (!IsLocked) {
    Renderer->DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
    if (IsSplit) {
      Renderer->DrawSprite(
          BottomPart,
          glm::vec2(Bounds.X, Bounds.Y + NormalSprite.ScaledHeight()), Tint);
    }
  } else {
    Renderer->DrawSprite(DisabledSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  }
  if (HasFocus) {
    auto offset = 2.0f * glm::step(0.5f, HighlightAnimation.Progress);
    Renderer->DrawSprite(
        HighlightTopLeft,
        glm::vec2(Bounds.X - HighlightTopLeftOffset.x + offset,
                  Bounds.Y - HighlightTopLeftOffset.y + offset),
        Tint);
    Renderer->DrawSprite(
        HighlightTopRight,
        glm::vec2(Bounds.X + Bounds.Width - HighlightTopRightOffset.x - offset,
                  Bounds.Y - HighlightTopRightOffset.y + offset),
        Tint);
    Renderer->DrawSprite(
        HighlightBottomLeft,
        glm::vec2(
            Bounds.X - HighlightBottomLeftOffset.x + offset,
            Bounds.Y + Bounds.Height - HighlightBottomLeftOffset.y - offset),
        Tint);
    Renderer->DrawSprite(
        HighlightBottomRight,
        glm::vec2(
            Bounds.X + Bounds.Width - HighlightBottomRightOffset.x - offset,
            Bounds.Y + Bounds.Height - HighlightBottomRightOffset.y - offset),
        Tint);
  }
}

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto