#pragma once

#include "../widget.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace Widgets {

enum ScrollbarDirection { SBDIR_VERTICAL, SBDIR_HORIZONTAL };

class Scrollbar : public Widget {
 public:
  Scrollbar(int id, glm::vec2 pos, float start, float end, float* value,
            ScrollbarDirection dir, glm::vec2 trackBounds);
  Scrollbar(int id, glm::vec2 pos, float start, float end, float* value,
            ScrollbarDirection dir, Sprite const& thumb, glm::vec2 trackBounds,
            float thumbLength, RectF wheelBounds = RectF(),
            float wheelSpeedMultiplier = 1.0f);
  Scrollbar(int id, glm::vec2 pos, float start, float end, float* value,
            ScrollbarDirection dir, Sprite const& track, Sprite const& thumb,
            glm::vec2 thumbOffset = glm::vec2(0.0f, 0.0f),
            float thumbLength = 0.0f, RectF wheelBounds = RectF(),
            float wheelSpeedMultiplier = 1.0f);
  Scrollbar(int id, glm::vec2 pos, float start, float end, float* value,
            ScrollbarDirection dir, Sprite const& track, Sprite const& thumb,
            Sprite const& fill, glm::vec2 thumbOffset = glm::vec2(0.0f, 0.0f),
            float thumbLength = 0.0f, RectF wheelBounds = RectF(),
            float wheelSpeedMultiplier = 1.0f);
  virtual void UpdateInput(float dt) override;
  virtual void Update(float dt) override;
  virtual void Render() override;

  void Move(glm::vec2 relativePosition) override;

  void ClampValue();
  float GetNormalizedValue() const {
    return (*Value - StartValue) / (EndValue - StartValue);
  };

  RectF GetTrackBounds() const { return TrackBounds; }

  int Id;
  ScrollbarDirection Direction;
  Sprite ThumbSprite;
  std::optional<Sprite> TrackSprite;
  std::optional<Sprite> FillSprite;
  float Length;
  float StartValue;
  float EndValue;
  float* Value;

  bool FillBeforeTrack = false;
  bool HoveredWheelBounds = false;

 protected:
  float Step = 0.0f;
  float TrackProgress = 0.0f;
  glm::vec2 ThumbPosition;
  glm::vec2 ThumbSpriteOffset = glm::vec2(0.0f, 0.0f);
  RectF TrackBounds;
  RectF ThumbBounds;
  RectF ScrollWheelBounds;
  float ThumbLength;
  bool ScrollHeld = false;
  int LastScrollPos = 0;
  float WheelSpeedMultiplier = 1.0f;
  void UpdatePosition();
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto