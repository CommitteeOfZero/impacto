#pragma once

#include "util.h"

namespace Impacto {

struct SpriteSheet {
  SpriteSheet(float width, float height)
      : DesignWidth(width), DesignHeight(height) {}

  float DesignWidth;
  float DesignHeight;

  GLuint Texture = 0;
};

struct Sprite {
  Sprite(SpriteSheet const& sheet, float x, float y, float width, float height)
      : Sheet(sheet), Bounds(x, y, width, height) {}

  SpriteSheet const& Sheet;
  RectF Bounds;
};

}  // namespace Impacto