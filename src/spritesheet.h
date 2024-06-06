#pragma once

#include "util.h"

namespace Impacto {

struct SpriteSheet {
  SpriteSheet() {}
  SpriteSheet(float width, float height)
      : DesignWidth(width), DesignHeight(height) {}

  float DesignWidth;
  float DesignHeight;

  uint32_t Texture = 0;
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

  float ScaledWidth() { return Bounds.Width * BaseScale.x; }
  float ScaledHeight() { return Bounds.Height * BaseScale.y; }
  void SetScaledWidth(float scaledWidth) {
    Bounds.Width = scaledWidth / BaseScale.x;
  }
  void SetScaledHeight(float scaledHeight) {
    Bounds.Height = scaledHeight / BaseScale.y;
  }
};

}  // namespace Impacto