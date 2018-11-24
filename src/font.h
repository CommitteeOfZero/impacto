#pragma once

#include "spritesheet.h"

namespace Impacto {

class Font {
 public:
  Font() {}
  Font(float width, float height, uint8_t columns, uint8_t rows)
      : Sheet(width, height), Columns(columns), Rows(rows) {}

  SpriteSheet Sheet;
  uint8_t Columns;
  uint8_t Rows;
  float* Widths;

  float RowHeight() const { return Sheet.DesignHeight / (float)Rows; }
  float ColWidth() const { return Sheet.DesignWidth / (float)Columns; }

  Sprite Glyph(uint8_t row, uint8_t col) { return Glyph(row * Columns + col); }

  Sprite Glyph(uint16_t id) {
    uint8_t row = id / Columns;
    uint8_t col = id % Columns;
    float width = Widths[id];
    return Sprite(Sheet, col * ColWidth(), row * RowHeight(), width,
                  RowHeight());
  }
};

}  // namespace Impacto