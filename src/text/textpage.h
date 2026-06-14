#pragma once

#include "text.h"
#include "../audio/audiostream.h"
#include "../renderer/renderer.h"

namespace Impacto {

struct RubyChunk {
  size_t FirstBaseCharacter = 0;
  size_t BaseLength = 0;
  std::vector<ProcessedTextGlyph> Text;
  std::vector<uint16_t> RawText;
  bool CenterPerCharacter = false;

  void FinishBase(size_t glyphCount) {
    BaseLength = glyphCount - FirstBaseCharacter;
  }
};

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

  RectF Bounds;

  float CurrentLineTop = 0.0f;
  float CurrentLineTopMargin = 0.0f;

 protected:
  TextPage() = default;

  virtual RectF SetBounds();

  size_t LastLineStart = 0;
};

struct TextModeInfo {
  uint16_t DisplayMode = 0;
  size_t WindowId = 0;

  glm::vec2 WindowPos = {0.0f, 0.0f};

  enum class NameDispModeType : uint8_t {
    RelativeToWindow = 0,
    Invisible = 1,
    InText = 2,
    FixedPos = 3,
  };
  NameDispModeType NameDispMode = NameDispModeType::RelativeToWindow;

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
  bool AlwaysAddRubySpacing = false;
  float LinefeedSpacing = 0.0f;

  enum class NameAlignmentType : uint8_t {
    Left = 0,
    Center = 1,
    Right = 2,
  };
  NameAlignmentType NameAlignment = NameAlignmentType::Center;

  // Idk what these are either
  bool UseNameLengthL = false;
  uint16_t NameLengthL = 0;
};
inline std::array<TextModeInfo, 10> TextModesInfo;

}  // namespace Impacto
