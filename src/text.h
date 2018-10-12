#pragma once

#include "spritesheet.h"
#include "vm/thread.h"

namespace Impacto {

enum TextAlignment { TA_Left, TA_Center };

// TODO: think about / profile memory access patterns

// currently exactly 64 bytes on 64-bit
struct ProcessedTextGlyph {
  glm::vec4 Color;
  Sprite Glyph;
  float Opacity;
  RectF DestRect;
};

enum DialoguePageMode : uint8_t { DPM_ADV = 0, DPM_NVL = 1 };

int const DialogueMaxNameLength = 32;
int const DialogueMaxRubyChunks = 32;
int const DialogueMaxRubyChunkLength = 32;

struct DialoguePageFeatureConfig {
  RectF NVLBounds;
  RectF ADVBounds;
  glm::vec4 OutlineColor;
  float DefaultFontSize;
};

struct RubyChunk {
  int FirstBaseCharacter;
  int Length;
  uint16_t Text[DialogueMaxRubyChunkLength];
};

struct DialoguePage {
  static int const MaxCharacters = 2000;

  Game* GameCtx;
  Font* FontHandle;

  int Id;

  int Length;
  int FullyOpaqueGlyphCount;
  bool IsFullyOpaque;

  int NameLength;
  bool HasName;
  uint16_t Name[DialogueMaxNameLength];

  int RubyChunkCount;
  int CurrentRubyChunk;
  RubyChunk RubyChunks[DialogueMaxRubyChunks];

  float CurrentX;
  float CurrentY;

  ProcessedTextGlyph Glyphs[MaxCharacters];

  DialoguePageMode Mode;

  bool NVLResetBeforeAdd;
  bool AutoForward;

  void Clear();
  void AddString(Vm::Sc3VmThread* ctx);
  glm::vec4 GetCurrentBaseColor();
};

int TextGetStringLength(Vm::Sc3VmThread* ctx);

}  // namespace Impacto