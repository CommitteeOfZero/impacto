#pragma once

#include "text.h"
#include "../audio/audiostream.h"
#include "../renderer/renderer.h"

namespace Impacto {

class TextPage {
 public:
  virtual ~TextPage() = default;

  virtual void Clear();

  virtual void Move(glm::vec2 relativePos);
  void MoveTo(glm::vec2 pos);

  virtual void Render(
      float alpha, RendererOutlineMode outlineMode = RendererOutlineMode::Full);

  std::vector<RubyChunk> RubyChunks;
  std::vector<ProcessedTextGlyph> Glyphs;

  RectF BoxBounds;

  glm::vec2 Dimensions;
  size_t Length = 0;
  float FontSize;

  TextAlignment Alignment = TextAlignment::Left;

  float CurrentLineTop = 0.0f;
  float CurrentLineTopMargin = 0.0f;

 protected:
  TextPage() = default;

  size_t LastLineStart = 0;
};

struct TextModeInfo {
  uint16_t DisplayMode = 0;
  size_t WindowId = 0;

  glm::vec2 WindowPos = {0.0f, 0.0f};

  uint16_t NameDispMode = 0;
  float MaxNameWidth = 0.0f;
  glm::vec2 NamePos = {0.0f, 0.0f};
  glm::vec2 NameGlyphSize = {0.0f, 0.0f};

  float MaxLineWidth = 0;

  size_t CurrentPageId = 0;
  glm::vec2 WaitIconPos = {0.0f, 0.0f};

  glm::vec2 TextGlyphSize = {0.0f, 0.0f};
  glm::vec2 RubyGlyphSize = {0.0f, 0.0f};

  float LineSpacing = 0.0f;
  float RubyLineSpacing = 0.0f;
  float LinefeedSpacing = 0.0f;

  uint16_t NamePosFlags = 0;
  uint16_t NameLengthL = 0;  // Idk what this is either
};
inline std::array<TextModeInfo, 10> TextModesInfo;

}  // namespace Impacto
