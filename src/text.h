#pragma once

#include "font.h"
#include "animation.h"
#include "vm/thread.h"
#include <enum.h>

#include "audio/audiosystem.h"
#include "audio/audiostream.h"
#include "audio/audiochannel.h"

namespace Impacto {

BETTER_ENUM(TextAlignment, int, Left = 0, Center, Right, Block)
// Block alignment only supported for ruby

BETTER_ENUM(CharacterTypeFlags, uint8_t, Space = (1 << 0),
            WordStartingPunct = (1 << 1), WordEndingPunct = (1 << 2))

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

enum DialoguePageMode : uint8_t { DPM_ADV = 0, DPM_NVL = 1 };

int const DialogueMaxNameLength = 32;
int const DialogueMaxRubyChunks = 32;
int const DialogueMaxRubyChunkLength = 32;

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

  Animation FadeAnimation;
  TypewriterEffect Typewriter;
  // TODO get rid of this
  bool TextIsFullyOpaque();

  int Length;

  int NameLength;
  bool HasName;
  ProcessedTextGlyph Name[DialogueMaxNameLength];

  int RubyChunkCount;
  int CurrentRubyChunk;
  RubyChunk RubyChunks[DialogueMaxRubyChunks];

  ProcessedTextGlyph* Glyphs;

  DialoguePageMode Mode;

  bool NVLResetBeforeAdd;
  bool AutoForward;

  void Clear();
  void AddString(Vm::Sc3VmThread* ctx, Audio::AudioStream* voice = 0);
  void Update(float dt);
  void Render();

 private:
  void FinishLine(Vm::Sc3VmThread* ctx, int nextLineStart);
  void EndRubyBase(int lastBaseCharacter);

  bool BuildingRubyBase;
  int FirstRubyChunkOnLine;

  float CurrentLineTop;
  float CurrentLineTopMargin;
  int LastLineStart;

  DialoguePageMode PrevMode = DPM_ADV;
};

extern DialoguePage* DialoguePages;

int TextGetStringLength(Vm::Sc3VmThread* ctx);
int TextGetMainCharacterCount(Vm::Sc3VmThread* ctx);
int TextLayoutPlainLine(Vm::Sc3VmThread* ctx, int stringLength,
                        ProcessedTextGlyph* outGlyphs, Font* font,
                        float fontSize, DialogueColorPair colors, float opacity,
                        glm::vec2 pos, TextAlignment alignment,
                        float blockWidth = 0.0f);

}  // namespace Impacto