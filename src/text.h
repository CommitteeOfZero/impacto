#pragma once

#include <set>
#include <span>
#include <enum.h>
#include <ankerl/unordered_dense.h>

#include "font.h"
#include "animation.h"
#include "vm/thread.h"
#include "vm/sc3stream.h"
#include "audio/audiosystem.h"
#include "audio/audiostream.h"
#include "audio/audiochannel.h"

namespace Impacto {

BETTER_ENUM(TextAlignment, int, Left = 0, Center, Right, Block)
// Block alignment only supported for ruby

BETTER_ENUM(CharacterTypeFlags, uint8_t, Space = (1 << 0),
            WordStartingPunct = (1 << 1), WordEndingPunct = (1 << 2))

BETTER_ENUM(SkipModeFlags, uint8_t, SkipRead = (1 << 0), SkipAll = (1 << 1),
            Auto = (1 << 2))

// TODO: think about / profile memory access patterns

struct DialogueColorPair {
  uint32_t TextColor;
  uint32_t OutlineColor;
};

struct ProcessedTextGlyph {
  DialogueColorPair Colors;
  uint16_t CharId;
  float Opacity;
  RectF DestRect;

  static inline ankerl::unordered_dense::map<uint16_t, uint8_t> FlagsMap;
  static void AddFlags(Vm::BufferOffsetContext scrCtx, uint8_t flags);

  uint8_t Flags() const {
    return FlagsMap.contains(CharId) ? FlagsMap[CharId] : 0;
  }
};

enum DialoguePageMode : uint8_t {
  DPM_ADV = 0,
  DPM_NVL = 1,
  DPM_REV = 2,
  DPM_TIPS = 3
};

int constexpr DialogueMaxNameLength = 64;
int constexpr DialogueMaxRubyChunks = 32;
int constexpr DialogueMaxRubyChunkLength = 32;

struct RubyChunk {
  size_t FirstBaseCharacter;
  size_t Length;
  size_t BaseLength;
  bool CenterPerCharacter;
  std::array<ProcessedTextGlyph, DialogueMaxRubyChunkLength> Text;
  std::array<uint16_t, DialogueMaxRubyChunkLength> RawText;
};

struct TypewriterEffect : public Animation {
 public:
  void Start(size_t firstGlyph, size_t glyphCount,
             const std::set<size_t>& parallelStartGlyphs, bool voiced);
  void Update(float dt);

  float CalcOpacity(size_t glyph);
  float CalcRubyOpacity(size_t rubyGlyphId, const RubyChunk& chunk);

  size_t GetGlyphCount() const { return GlyphCount; }

  bool CancelRequested = false;
  bool IsCancelled = false;

 private:
  size_t FirstGlyph = 0;
  size_t GlyphCount = 0;

  bool Voiced = false;

  std::set<size_t> ParallelStartGlyphs;
  float ProgressOnCancel;

  struct ParallelBlock {
    size_t Start;
    size_t Size;
  };
  ParallelBlock GetParallelBlock(size_t glyph);

  // {startProgress, endProgress}
  std::pair<float, float> GetGlyphWritingProgresses(size_t glyph);
};

struct DialoguePage {
  static void Init();

  int Id;
  int AnimationId;

  Animation FadeAnimation;
  TypewriterEffect Typewriter;
  // TODO get rid of this
  bool TextIsFullyOpaque();

  int Length;
  glm::vec2 Dimensions;
  float FontSize;

  size_t NameLength;
  int NameId;
  bool HasName;
  std::vector<ProcessedTextGlyph> Name;

  size_t RubyChunkCount;
  int CurrentRubyChunk;
  std::array<RubyChunk, DialogueMaxRubyChunks> RubyChunks;

  std::vector<ProcessedTextGlyph> Glyphs;

  DialoguePageMode Mode;
  TextAlignment Alignment = TextAlignment::Left;

  bool NVLResetBeforeAdd;

  enum class AutoForwardType { Off, Normal, SyncVoice };
  AutoForwardType AutoForward = AutoForwardType::Off;
  float AutoWaitTime = 0.0f;

  RectF BoxBounds;

  void Clear();
  void AddString(Vm::Sc3VmThread* ctx, Audio::AudioStream* voice = 0,
                 int animId = 0);
  void Update(float dt);
  void Move(glm::vec2 relativePos);
  void MoveTo(glm::vec2 pos);
  void Render();

 private:
  void FinishLine(Vm::Sc3VmThread* ctx, size_t nextLineStart,
                  const RectF& boxBounds, TextAlignment alignment);
  void EndRubyBase(int lastBaseCharacter);

  bool BuildingRubyBase;
  size_t FirstRubyChunkOnLine;

  float CurrentLineTop;
  float CurrentLineTopMargin;
  size_t LastLineStart;
  DialoguePageMode PrevMode = DPM_ADV;
};

inline DialoguePage* DialoguePages;
inline int DialoguePageCount = 0;

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
uint32_t GetNameId(uint8_t* name, int nameLength);

// Bitfield denoting the skip mode, according to SkipModeFlags
inline uint8_t MesSkipMode = 0;

}  // namespace Impacto