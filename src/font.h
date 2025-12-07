#pragma once

#include <vector>

#include <enum.h>

#include "spritesheet.h"

namespace Impacto {

BETTER_ENUM(FontType, int, Basic, LB)

class Font {
 public:
  Font(FontType type) : Type(type) {}

  FontType Type;

  uint8_t Columns;
  uint8_t Rows;
  std::vector<float> AdvanceWidths;

  float CellHeight;
  float CellWidth;

  float BitmapEmWidth;
  float BitmapEmHeight;

  float LineSpacing;

  virtual void CalculateDefaultSizes() = 0;
};

class BasicFont : public Font {
 public:
  BasicFont() : Font(FontType::Basic) {}

  SpriteSheet Sheet;

  void CalculateDefaultSizes() override {
    CellHeight = Sheet.DesignHeight / (float)Rows;
    CellWidth = Sheet.DesignWidth / (float)Columns;
  }

  Sprite Glyph(uint8_t row, uint8_t col) { return Glyph(row * Columns + col); }

  Sprite Glyph(uint16_t id) {
    uint8_t row = (uint8_t)(id / Columns);
    uint8_t col = id % Columns;
    float width = AdvanceWidths[id];
    return Sprite(Sheet, col * CellWidth + 1, row * CellHeight + 1, width - 2,
                  BitmapEmHeight - 2);
  }
};

class LBFont : public Font {
 public:
  LBFont() : Font(FontType::LB) {}

  SpriteSheet ForegroundSheet;
  SpriteSheet OutlineSheet;

  float OutlineCellHeight;
  float OutlineCellWidth;

  glm::vec2 ForegroundOffset;
  glm::vec2 OutlineOffset;

  void CalculateDefaultSizes() override {
    CellHeight = ForegroundSheet.DesignHeight / (float)Rows;
    CellWidth = ForegroundSheet.DesignWidth / (float)Columns;

    OutlineCellHeight = OutlineSheet.DesignHeight / (float)Rows;
    OutlineCellWidth = OutlineSheet.DesignWidth / (float)Columns;
  }

  Sprite Glyph(uint8_t row, uint8_t col) { return Glyph(row * Columns + col); }

  Sprite Glyph(uint16_t id) {
    uint8_t row = (uint8_t)(id / Columns);
    uint8_t col = id % Columns;
    return Sprite(ForegroundSheet, col * CellWidth, row * CellHeight, CellWidth,
                  CellHeight);
  }

  Sprite OutlineGlyph(uint8_t row, uint8_t col) {
    return OutlineGlyph(row * Columns + col);
  }

  Sprite OutlineGlyph(uint16_t id) {
    uint8_t row = (uint8_t)(id / Columns);
    uint8_t col = id % Columns;
    return Sprite(OutlineSheet, col * OutlineCellWidth, row * OutlineCellHeight,
                  OutlineCellWidth, OutlineCellHeight);
  }
};

}  // namespace Impacto