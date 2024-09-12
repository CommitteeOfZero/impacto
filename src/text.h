#pragma once

#include "font.h"
#include "animation.h"
#include "vm/thread.h"
#include <enum.h>
#include <flat_hash_map.hpp>

#include "audio/audiosystem.h"
#include "audio/audiostream.h"
#include "audio/audiochannel.h"
#include "../vendor/span/span.hpp"

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

  uint8_t Flags() const;
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
  int FirstBaseCharacter;
  int Length;
  int BaseLength;
  bool CenterPerCharacter;
  ProcessedTextGlyph Text[DialogueMaxRubyChunkLength];
  uint16_t RawText[DialogueMaxRubyChunkLength];
};

struct TypewriterEffect : public Animation {
 public:
  void Start(int firstGlyph, int glyphCount, float duration);
  float CalcOpacity(int glyph);
  void Update(float dt);
  int LastOpaqueCharacter;
  bool CancelRequested = false;
  bool IsCancelled = false;

 private:
  int FirstGlyph;
  int GlyphCount;
  float CancelStartTime;
};

struct DialoguePage {
  static void Init();

  int Id;
  int AnimationId;

  Animation FadeAnimation;
  TypewriterEffect Typewriter;
  // TODO get rid of this
  bool TextIsFullyOpaque();

  int NameLength;
  int NameId;
  bool HasName;
  std::vector<ProcessedTextGlyph> Name;

  int RubyChunkCount;
  int CurrentRubyChunk;
  RubyChunk RubyChunks[DialogueMaxRubyChunks];

  std::vector<ProcessedTextGlyph> Glyphs;

  DialoguePageMode Mode;

  bool NVLResetBeforeAdd;
  bool AutoForward;

  float AutoWaitTime;

  RectF BoxBounds;

  void Clear();
  void AddString(Vm::Sc3VmThread* ctx, Audio::AudioStream* voice = 0,
                 int animId = 0);
  void Update(float dt);
  void Move(glm::vec2 relativePos);
  void MoveTo(glm::vec2 pos);
  void Render();

 private:
  void FinishLine(Vm::Sc3VmThread* ctx, int nextLineStart,
                  const RectF& boxBounds, TextAlignment alignment);
  void EndRubyBase(int lastBaseCharacter);

  bool BuildingRubyBase;
  int FirstRubyChunkOnLine;

  float CurrentLineTop;
  float CurrentLineTopMargin;
  int LastLineStart;
  DialoguePageMode PrevMode = DPM_ADV;
};

inline DialoguePage* DialoguePages;
inline int DialoguePageCount = 0;

int TextGetStringLength(Vm::Sc3VmThread* ctx);
int TextGetMainCharacterCount(Vm::Sc3VmThread* ctx);
int TextLayoutPlainLine(Vm::Sc3VmThread* ctx, int stringLength,
                        tcb::span<ProcessedTextGlyph> outGlyphs, Font* font,
                        float fontSize, DialogueColorPair colors, float opacity,
                        glm::vec2 pos, TextAlignment alignment,
                        float blockWidth = 0.0f);
std::vector<ProcessedTextGlyph> TextLayoutPlainLine(
    Vm::Sc3VmThread* ctx, int maxLength, Font* font, float fontSize,
    DialogueColorPair colors, float opacity, glm::vec2 pos,
    TextAlignment alignment, float blockWidth = 0.0f);
int TextLayoutAlignment(Impacto::TextAlignment& alignment, float blockWidth,
                        float currentX, glm::vec2& pos, int characterCount,
                        tcb::span<Impacto::ProcessedTextGlyph> outGlyphs);
float TextGetPlainLineWidth(Vm::Sc3VmThread* ctx, Font* font, float fontSize);
int TextLayoutPlainString(std::string_view str,
                          tcb::span<ProcessedTextGlyph> outGlyphs, Font* font,
                          float fontSize, DialogueColorPair colors,
                          float opacity, glm::vec2 pos, TextAlignment alignment,
                          float blockWidth = 0.0f);
std::vector<ProcessedTextGlyph> TextLayoutPlainString(
    std::string_view str, Font* font, float fontSize, DialogueColorPair colors,
    float opacity, glm::vec2 pos, TextAlignment alignment,
    float blockWidth = 0.0f);

void TextGetSc3String(std::string_view str, tcb::span<uint16_t> out);

inline ska::flat_hash_map<uint32_t, uint32_t> NamePlateData;
void InitNamePlateData(uint16_t* data);
uint32_t GetNameId(uint8_t* name, int nameLength);

// Bitfield denoting the skip mode, according to SkipModeFlags
inline uint8_t MesSkipMode = 0;

// Speed to skip in auto mode (MessWaitSpeed)
inline float AutoSpeed = 768 / 60;
inline bool SkipMode = false;  // Skip unread text

// Stop skip mode when reaching a trigger
// (e.g. delusion trigger, phone trigger, etc.)
inline bool TriggerStopSkip = true;

}  // namespace Impacto