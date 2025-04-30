#pragma once

#include "util.h"

namespace Impacto {

struct SpriteSheet {
  SpriteSheet() {}
  SpriteSheet(float width, float height)
      : DesignWidth(width), DesignHeight(height) {}

  float DesignWidth = 0;
  float DesignHeight = 0;

  glm::vec2 GetDimensions() const { return {DesignWidth, DesignHeight}; }

  uint32_t Texture = 0;
  bool IsScreenCap = false;
};

// TODO replace BaseScale with scaled width/height and unscaled width/height

struct Sprite {
  Sprite() : BaseScale(1.0f) {}
  Sprite(SpriteSheet const& sheet, float x, float y, float width, float height,
         glm::vec2 baseScale = glm::vec2(1.0f))
      : Sheet(sheet), Bounds(x, y, width, height), BaseScale(baseScale) {}

  SpriteSheet Sheet;
  RectF Bounds{0.0f, 0.0f, 0.0f, 0.0f};
  glm::vec2 BaseScale;

  float ScaledWidth() const { return Bounds.Width * BaseScale.x; }
  float ScaledHeight() const { return Bounds.Height * BaseScale.y; }
  void SetScaledWidth(float scaledWidth) {
    Bounds.Width = scaledWidth / BaseScale.x;
  }
  void SetScaledHeight(float scaledHeight) {
    Bounds.Height = scaledHeight / BaseScale.y;
  }
  RectF ScaledBounds() const {
    return RectF(Bounds).Scale(BaseScale, Bounds.GetPos());
  }
};

}  // namespace Impacto