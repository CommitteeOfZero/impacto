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
  Sprite() {}
  Sprite(SpriteSheet const& sheet, float x, float y, float width, float height)
      : Sheet(sheet), Bounds(x, y, width, height) {}

  SpriteSheet Sheet;
  RectF Bounds;
};

// TODO dataify and move out of here, this file is supposed to be graphics code
enum CharacterTypeFlags : uint8_t {
  CTF_Space = (1 << 0),
  CTF_WordStartingPunct = (1 << 1),
  CTF_WordEndingPunct = (1 << 2)
};

struct Font {
  Font() {}
  Font(float width, float height, uint8_t columns, uint8_t rows)
      : Sheet(width, height), Columns(columns), Rows(rows) {}

  SpriteSheet Sheet;
  uint8_t Columns;
  uint8_t Rows;

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

  // TODO again, dataify and move out of here
  uint8_t CharacterType(uint16_t glyphId) {
    uint8_t result = 0;
    if (glyphId == 0 || glyphId == 63) result |= CTF_Space;

    // TODO check if these are even the right ones for R;NE

    // 、 。 ． ， ？ ！ 〜 ” ー ） 〕 ］ ｝ 〉 》 」 』 】☆ ★ ♪ 々 ぁ ぃ ぅ ぇ
    // ぉ っ ゃ ゅ ょ ァ ィ ゥ ェ ォ ッ ャ ュ ョ –
    if (glyphId == 0x00BE || glyphId == 0x00BF || glyphId == 0x00C1 ||
        glyphId == 0x00C0 || glyphId == 0x00C4 || glyphId == 0x00C5 ||
        glyphId == 0x00E4 || glyphId == 0x00CB || glyphId == 0x00E5 ||
        glyphId == 0x00CD || glyphId == 0x00CF || glyphId == 0x00D1 ||
        glyphId == 0x00D3 || glyphId == 0x00D5 || glyphId == 0x00D7 ||
        glyphId == 0x00D9 || glyphId == 0x00DB || glyphId == 0x00DD ||
        glyphId == 0x01A5 || glyphId == 0x01A6 || glyphId == 0x00E6 ||
        glyphId == 0x0187 || glyphId == 0x00E8 || glyphId == 0x00E9 ||
        glyphId == 0x00EA || glyphId == 0x00EB || glyphId == 0x00EC ||
        glyphId == 0x00ED || glyphId == 0x00EE || glyphId == 0x00EF ||
        glyphId == 0x00F0 || glyphId == 0x00F2 || glyphId == 0x00F3 ||
        glyphId == 0x00F4 || glyphId == 0x00F5 || glyphId == 0x00F6 ||
        glyphId == 0x00F7 || glyphId == 0x00F8 || glyphId == 0x00F9 ||
        glyphId == 0x00FA || glyphId == 0x0113)
      result |= CTF_WordEndingPunct;

    // space(63) space(0) “ （ 〔 ［ ｛ 〈 《 「
    if (glyphId == 63 || glyphId == 0 || glyphId == 0x00CA ||
        glyphId == 0x00CC || glyphId == 0x00CE || glyphId == 0x00D0 ||
        glyphId == 0x00D2 || glyphId == 0x00D4 || glyphId == 0x00D6 ||
        glyphId == 0x00D8 || glyphId == 0x00DA || glyphId == 0x00DC)
      result |= CTF_WordStartingPunct;

    return result;
  }
};

}  // namespace Impacto