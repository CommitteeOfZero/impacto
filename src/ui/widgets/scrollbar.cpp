#include "scrollbar.h"
#include "../../renderer/renderer.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace Widgets {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

Scrollbar::Scrollbar(int id, glm::vec2 pos, float start, float end,
                     float* value, ScrollbarDirection dir, Sprite const& thumb,
                     glm::vec2 trackBounds, float thumbLength)
    : Id(id),
      StartValue(start),
      EndValue(end),
      Value(value),
      Direction(dir),
      ThumbSprite(thumb),
      TrackBounds(pos.x, pos.y, trackBounds.x, trackBounds.y),
      ThumbLength(thumbLength) {
  Enabled = true;
  Step = (EndValue - StartValue) * 0.01f;
  Length = Direction == SBDIR_VERTICAL ? trackBounds.y : trackBounds.x;
  UpdatePosition();
}

Scrollbar::Scrollbar(int id, glm::vec2 pos, float start, float end,
                     float* value, ScrollbarDirection dir, Sprite const& track,
                     Sprite const& thumb, glm::vec2 thumbOffset,
                     float thumbLength)
    : Id(id),
      StartValue(start),
      EndValue(end),
      Value(value),
      Direction(dir),
      TrackSprite(track),
      ThumbSprite(thumb),
      ThumbSpriteOffset(thumbOffset),
      ThumbLength(thumbLength),
      HasTrack(true) {
  Enabled = true;
  Step = (EndValue - StartValue) * 0.01f;
  Length = Direction == SBDIR_VERTICAL ? TrackSprite.Bounds.Height
                                       : TrackSprite.Bounds.Width;
  TrackBounds = RectF(pos.x, pos.y, TrackSprite.ScaledWidth(),
                      TrackSprite.ScaledHeight());
  ThumbBounds =
      RectF(0.0f, 0.0f, ThumbSprite.ScaledWidth(), ThumbSprite.ScaledHeight());
  if (Direction == SBDIR_VERTICAL) {
    ThumbBounds.X = (TrackBounds.X + (TrackBounds.Width / 2.0f)) -
                    (ThumbSprite.ScaledWidth() / 2.0f);
    ThumbBounds.Y = TrackBounds.Y - (ThumbSprite.ScaledHeight() / 2.0f);
  } else if (Direction == SBDIR_HORIZONTAL) {
    ThumbBounds.X = TrackBounds.X - (ThumbSprite.ScaledWidth() / 2.0f);
    ThumbBounds.Y = (TrackBounds.Y + (TrackBounds.Height / 2.0f)) -
                    (ThumbSprite.ScaledHeight() / 2.0f);
  }
  UpdatePosition();
}

Scrollbar::Scrollbar(int id, glm::vec2 pos, float start, float end,
                     float* value, ScrollbarDirection dir, Sprite const& track,
                     Sprite const& thumb, Sprite const& fill,
                     glm::vec2 thumbOffset, float thumbLength)
    : Scrollbar(id, pos, start, end, value, dir, track, thumb, thumbOffset,
                thumbLength) {
  FillSprite = fill;
  HasFill = true;
  UpdatePosition();
}

void Scrollbar::UpdateInput() {
  if (Enabled) {
    if (HasFocus) {
      switch (Direction) {
        case SBDIR_VERTICAL:
          if (PADinputButtonIsDown & PAD1DOWN) {
            *Value += Step;
          } else if (PADinputButtonIsDown & PAD1UP) {
            *Value -= Step;
          }
          break;
        case SBDIR_HORIZONTAL:
          if (PADinputButtonIsDown & PAD1RIGHT) {
            *Value += Step;
          } else if (PADinputButtonIsDown & PAD1LEFT) {
            *Value -= Step;
          }
          break;
      }
    }

    if (Input::PrevMousePos != Input::CurMousePos) {
      Hovered = TrackBounds.ContainsPoint(Input::CurMousePos) ||
                ThumbBounds.ContainsPoint(Input::CurMousePos);
    }
    if (Hovered && Input::MouseButtonIsDown[SDL_BUTTON_LEFT]) {
      Scrolling = true;
    }
    if (Input::MouseButtonIsDown[SDL_BUTTON_LEFT] && Scrolling) {
      float mouseP, trackP1, trackP2;
      switch (Direction) {
        case SBDIR_VERTICAL:
          mouseP = Input::CurMousePos.y;
          trackP1 = TrackBounds.Y;
          trackP2 = TrackBounds.Height;
          break;
        case SBDIR_HORIZONTAL:
          mouseP = Input::CurMousePos.x;
          trackP1 = TrackBounds.X;
          trackP2 = TrackBounds.Width;
          break;
      }

      float thumbNormalizedLength =
          (trackP2 - ThumbLength) / (EndValue - StartValue);
      *Value = StartValue + ((mouseP - (trackP1 + ThumbLength / 2.0f)) /
                             thumbNormalizedLength);

      float minValue = std::min(StartValue, EndValue);
      float maxValue = std::max(StartValue, EndValue);
      *Value = std::clamp(*Value, minValue, maxValue);
    } else {
      Scrolling = false;
    }
  }
}

void Scrollbar::Render() {
  if (HasFill && FillBeforeTrack) {
    Renderer->DrawSprite(FillSprite, glm::vec2(TrackBounds.X, TrackBounds.Y),
                         Tint);
  }
  if (HasTrack) {
    Renderer->DrawSprite(TrackSprite, TrackBounds, Tint);
  }
  if (HasFill && !FillBeforeTrack) {
    Renderer->DrawSprite(FillSprite, glm::vec2(TrackBounds.X, TrackBounds.Y),
                         Tint);
  }
  Renderer->DrawSprite(ThumbSprite, glm::vec2(ThumbBounds.X, ThumbBounds.Y),
                       Tint);
}

void Scrollbar::Move(glm::vec2 relativePosition) {
  TrackBounds.X += relativePosition.x;
  TrackBounds.Y += relativePosition.y;
  ThumbBounds.X += relativePosition.x;
  ThumbBounds.Y += relativePosition.y;
}

void Scrollbar::MoveTo(glm::vec2 pos) {
  auto relativePosition = pos - glm::vec2(Bounds.X, Bounds.Y);
  Move(relativePosition);
}

void Scrollbar::Update(float dt) { UpdatePosition(); }

void Scrollbar::UpdatePosition() {
  TrackProgress = ((*Value - StartValue) / (EndValue - StartValue)) * Length;
  if (TrackProgress > Length) {
    *Value = EndValue;
    TrackProgress = Length;
  } else if (TrackProgress < 0.0f) {
    *Value = StartValue;
    TrackProgress = 0.0f;
  }

  float thumbNormalizedProgress =
      (TrackProgress / Length) * (Length - ThumbLength);
  if (Direction == SBDIR_VERTICAL) {
    ThumbBounds.X = (TrackBounds.X + (TrackBounds.Width / 2.0f)) -
                    (ThumbSprite.ScaledWidth() / 2.0f);
    ThumbBounds.Y =
        (TrackBounds.Y + ThumbLength / 2.0f + thumbNormalizedProgress) -
        (ThumbSprite.ScaledHeight() / 2.0f);
    FillSprite.Bounds.Height = TrackProgress;
  } else if (Direction == SBDIR_HORIZONTAL) {
    ThumbBounds.X =
        (TrackBounds.X + ThumbLength / 2.0f + thumbNormalizedProgress) -
        (ThumbSprite.ScaledWidth() / 2.0f);
    ThumbBounds.Y = (TrackBounds.Y + (TrackBounds.Height / 2.0f)) -
                    (ThumbSprite.ScaledHeight() / 2.0f);
    FillSprite.Bounds.Width = TrackProgress;
  }

  ThumbBounds.X += ThumbSpriteOffset.x;
  ThumbBounds.Y += ThumbSpriteOffset.y;
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto