#pragma once

#include "util.h"

namespace Impacto {

struct SpriteSheet {
  SpriteSheet() {}
  SpriteSheet(float width, float height)
      : DesignWidth(width), DesignHeight(height) {}

  float DesignWidth;
  float DesignHeight;

  GLuint Texture = 0;
};

// TODO replace BaseScale with scaled width/height and unscaled width/height

struct Sprite {
  Sprite() : BaseScale(1.0f) {}
  Sprite(SpriteSheet const& sheet, float x, float y, float width, float height,
         glm::vec2 baseScale = glm::vec2(1.0f))
      : Sheet(sheet), Bounds(x, y, width, height), BaseScale(baseScale) {}

  SpriteSheet Sheet;
  RectF Bounds;
  glm::vec2 BaseScale;
};

}  // namespace Impacto