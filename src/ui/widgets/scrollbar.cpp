#include "scrollbar.h"
#include "../../renderer2d.h"
#include "../../inputsystem.h"

namespace Impacto {
namespace UI {
namespace Widgets {

using namespace Impacto::Profile::ScriptVars;

Scrollbar::Scrollbar(int id, glm::vec2 pos, float min, float max, float* value,
                     ScrollbarDirection dir, Sprite const& track,
                     Sprite const& thumb) {
  Enabled = true;
  Id = id;
  MinValue = min;
  MaxValue = max;
  Value = value;
  *Value = min;
  TrackSprite = track;
  ThumbSprite = thumb;
  Direction = dir;
  Length = Direction == SBDIR_VERTICAL ? TrackSprite.Bounds.Height
                                       : TrackSprite.Bounds.Width;
  TrackBounds =
      RectF(pos.x, pos.y, TrackSprite.Bounds.Width, TrackSprite.Bounds.Height);
}

void Scrollbar::UpdateInput() {
  if (Enabled) {
    if (Input::PrevMousePos != Input::CurMousePos) {
      Hovered = TrackBounds.ContainsPoint(Input::CurMousePos) ||
                ThumbSprite.Bounds.ContainsPoint(Input::CurMousePos);
    }
    if (Hovered && Input::MouseButtonIsDown[SDL_BUTTON_LEFT]) {
      Scrolling = true;
    }
    if (Input::MouseButtonIsDown[SDL_BUTTON_LEFT] && Scrolling) {
      float mouseP, trackP1, trackP2;
      if (Direction == SBDIR_VERTICAL) {
        mouseP = Input::CurMousePos.y;
        trackP1 = TrackBounds.Y;
        trackP2 = TrackBounds.Height;
      } else if (Direction == SBDIR_HORIZONTAL) {
        mouseP = Input::CurMousePos.x;
        trackP1 = TrackBounds.X;
        trackP2 = TrackBounds.Width;
      }

      if (mouseP > trackP1 + trackP2)
        *Value = MaxValue;
      else if (mouseP < trackP1)
        *Value = MinValue;
      else {
        *Value =
            MinValue + (((mouseP - trackP1) / trackP2) * (MaxValue - MinValue));
      }
    } else {
      Scrolling = false;
    }
  }
}

void Scrollbar::Render() {
  Renderer2D::DrawSprite(TrackSprite, TrackBounds);
  float thumbX, thumbY;
  if (Direction == SBDIR_VERTICAL) {
    thumbX = (TrackBounds.X + (TrackBounds.Width / 2.0f)) -
             (ThumbSprite.ScaledWidth() / 2.0f);
    thumbY = (TrackBounds.Y +
              (((*Value - MinValue) / (MaxValue - MinValue)) * Length)) -
             (ThumbSprite.ScaledHeight() / 2.0f);
  } else if (Direction == SBDIR_HORIZONTAL) {
    thumbX = (TrackBounds.X +
              (((*Value - MinValue) / (MaxValue - MinValue)) * Length)) -
             (ThumbSprite.ScaledWidth() / 2.0f);
    thumbY = (TrackBounds.Y + (TrackBounds.Height / 2.0f)) -
             (ThumbSprite.ScaledHeight() / 2.0f);
  }
  Renderer2D::DrawSprite(ThumbSprite, glm::vec2(thumbX, thumbY));
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto