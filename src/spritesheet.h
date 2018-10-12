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

struct Sprite {
  Sprite(SpriteSheet const& sheet, float x, float y, float width, float height)
      : Sheet(sheet), Bounds(x, y, width, height) {}

  SpriteSheet Sheet;
  RectF Bounds;
};

struct Font {
  SpriteSheet Sheet;
  uint8_t Rows;
  uint8_t Columns;

  float RowHeight() const { return Sheet.DesignHeight / (float)Rows; }
  float ColWidth() const { return Sheet.DesignWidth / (float)Columns; }

  Sprite Glyph(uint8_t row, uint8_t col) {
    return Sprite(Sheet, col * ColWidth(), row * RowHeight(), ColWidth(),
                  RowHeight());
  }

  Sprite Glyph(uint16_t id) {
    uint8_t row = id / Columns;
    uint8_t col = id % Columns;
    return Glyph(row, col);
  }
};

}  // namespace Impacto