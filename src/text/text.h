#pragma once

#include <span>
#include <array>
#include <enum.h>
#include <ankerl/unordered_dense.h>

#include "../font.h"
#include "../animation.h"
#include "../vm/thread.h"
#include "../vm/sc3stream.h"

namespace Impacto {

BETTER_ENUM(TextAlignment, int, Left = 0, Center, Right, Block)
// Block alignment only supported for ruby

BETTER_ENUM(CharacterTypeFlags, uint8_t, Space = (1 << 0),
            WordStartingPunct = (1 << 1), WordEndingPunct = (1 << 2))

// TODO: think about / profile memory access patterns

enum StringTokenType : uint8_t {
  STT_LineBreak = 0x00,
  STT_CharacterNameStart = 0x01,
  STT_DialogueLineStart = 0x02,
  STT_Present = 0x03,
  STT_SetColor = 0x04,
  STT_Present_Clear = 0x08,
  STT_RubyBaseStart = 0x09,
  STT_RubyTextStart = 0x0A,
  STT_RubyTextEnd = 0x0B,
  STT_SetFontSize = 0x0C,
  STT_PrintInParallel = 0x0E,
  STT_CenterText = 0x0F,
  STT_SetTopMargin = 0x11,
  STT_SetLeftMargin = 0x12,
  STT_GetHardcodedValue = 0x13,
  STT_EvaluateExpression = 0x15,
  STT_UnlockTip = 0x16,
  STT_Present_0x18 = 0x18,
  STT_AutoForward_SyncVoice = 0x19,
  STT_AutoForward = 0x1A,
  STT_RubyCenterPerCharacter = 0x1E,
  STT_AltLineBreak = 0x1F,

  // This is our own!
  STT_Character = 0xFE,

  STT_EndOfString = 0xFF
};

struct StringToken {
 public:
  StringTokenType Type;

  uint16_t Val_Uint16;
  int Val_Expr;

  uint8_t Flags{};

  static void AddFlags(Vm::BufferOffsetContext scrCtx, uint8_t flags);
  static void AddFlags(uint16_t glyphId, uint8_t flags) {
    const auto found = FlagsMap.find(glyphId);
    if (found != FlagsMap.end()) {
      found->second |= flags;
    } else {
      StringToken::FlagsMap.emplace(glyphId, flags);
    }
  }
  static uint8_t GetFlags(uint16_t glyphId) {
    const auto found = FlagsMap.find(glyphId);
    return found == FlagsMap.end() ? 0 : found->second;
  }

  int Read(Vm::Sc3VmThread* ctx);
  int Read(Vm::Sc3Stream& stream);

 private:
  static inline ankerl::unordered_dense::map<uint16_t, uint8_t> FlagsMap;
};

struct DialogueColorPair {
  uint32_t TextColor;
  uint32_t OutlineColor;
};

struct ProcessedTextGlyph {
  DialogueColorPair Colors;
  uint16_t CharId;
  float Opacity;
  RectF DestRect;
};

struct RubyChunk {
  size_t FirstBaseCharacter;
  size_t Length;
  size_t BaseLength;
  std::array<ProcessedTextGlyph, 32> Text;
  std::array<uint16_t, 32> RawText;
  bool CenterPerCharacter;
};

int TextGetStringLength(Vm::Sc3Stream& stream);
int TextGetStringLength(Vm::Sc3VmThread* ctx);
[[maybe_unused]] int TextGetMainCharacterCount(Vm::Sc3VmThread* ctx);
int TextLayoutPlainLine(Vm::Sc3Stream& stream, int stringLength,
                        std::span<ProcessedTextGlyph> outGlyphs, Font* font,
                        float fontSize, DialogueColorPair colors, float opacity,
                        glm::vec2 pos, TextAlignment alignment,
                        float blockWidth = 0.0f);
int TextLayoutPlainLine(Vm::Sc3VmThread* ctx, int stringLength,
                        std::span<ProcessedTextGlyph> outGlyphs, Font* font,
                        float fontSize, DialogueColorPair colors, float opacity,
                        glm::vec2 pos, TextAlignment alignment,
                        float blockWidth = 0.0f);
std::vector<ProcessedTextGlyph> TextLayoutPlainLine(
    Vm::Sc3Stream& stream, int maxLength, Font* font, float fontSize,
    DialogueColorPair colors, float opacity, glm::vec2 pos,
    TextAlignment alignment, float blockWidth = 0.0f);
std::vector<ProcessedTextGlyph> TextLayoutPlainLine(
    Vm::Sc3VmThread* thd, int maxLength, Font* font, float fontSize,
    DialogueColorPair colors, float opacity, glm::vec2 pos,
    TextAlignment alignment, float blockWidth = 0.0f);
int TextLayoutAlignment(Impacto::TextAlignment& alignment, float blockWidth,
                        float currentX, glm::vec2& pos, int characterCount,
                        std::span<Impacto::ProcessedTextGlyph> outGlyphs);
float TextGetPlainLineWidth(Vm::Sc3VmThread* ctx, Font* font, float fontSize);
float TextGetPlainLineWidth(Vm::Sc3Stream& stream, Font* font, float fontSize);
int TextLayoutPlainString(std::string_view str,
                          std::span<ProcessedTextGlyph> outGlyphs, Font* font,
                          float fontSize, DialogueColorPair colors,
                          float opacity, glm::vec2 pos, TextAlignment alignment,
                          float blockWidth = 0.0f);
std::vector<ProcessedTextGlyph> TextLayoutPlainString(
    std::string_view str, Font* font, float fontSize, DialogueColorPair colors,
    float opacity, glm::vec2 pos, TextAlignment alignment,
    float blockWidth = 0.0f);

void TextGetSc3String(std::string_view str, std::span<uint16_t> out);

inline ankerl::unordered_dense::map<uint32_t, uint32_t> NamePlateData;
void InitNamePlateData(Vm::Sc3Stream& stream);
std::optional<uint32_t> GetNameId(std::span<const uint16_t> name);

}  // namespace Impacto