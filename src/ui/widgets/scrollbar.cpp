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

Scrollbar::Scrollbar(int id, glm::vec2 pos, float min, float max, float* value,
                     ScrollbarDirection dir, Sprite const& track,
                     Sprite const& thumb, Sprite const& fill)
    : Scrollbar(id, pos, min, max, value, dir, track, thumb) {
  FillSprite = fill;
  HasFill = true;
}

void Scrollbar::UpdateInput() {
  if (Enabled) {
    if (Input::PrevMousePos != Input::CurMousePos) {
      Hovered = TrackBounds.ContainsPoint(Input::CurMousePos) ||
                ThumbBounds.ContainsPoint(Input::CurMousePos);
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
  float part = ((*Value - MinValue) / (MaxValue - MinValue)) * Length;
  if (Direction == SBDIR_VERTICAL) {
    thumbX = (TrackBounds.X + (TrackBounds.Width / 2.0f)) -
             (ThumbSprite.ScaledWidth() / 2.0f);
    thumbY = (TrackBounds.Y + part) - (ThumbSprite.ScaledHeight() / 2.0f);
    if (HasFill) {
      FillSprite.Bounds.Height = part;
      Renderer2D::DrawSprite(FillSprite,
                             glm::vec2(TrackBounds.X, TrackBounds.Y), Tint);
    }
  } else if (Direction == SBDIR_HORIZONTAL) {
    thumbX = (TrackBounds.X + part) - (ThumbSprite.ScaledWidth() / 2.0f);
    thumbY = (TrackBounds.Y + (TrackBounds.Height / 2.0f)) -
             (ThumbSprite.ScaledHeight() / 2.0f);
    if (HasFill) {
      FillSprite.Bounds.Width = part;
      Renderer2D::DrawSprite(FillSprite,
                             glm::vec2(TrackBounds.X, TrackBounds.Y), Tint);
    }
  }
  ThumbBounds = RectF(thumbX, thumbY, ThumbSprite.ScaledWidth(),
                      ThumbSprite.ScaledHeight());

  Renderer2D::DrawSprite(ThumbSprite, glm::vec2(thumbX, thumbY), Tint);
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto