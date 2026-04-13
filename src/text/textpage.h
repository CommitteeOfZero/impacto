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

}  // namespace Impacto
