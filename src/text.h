#pragma once

#include "spritesheet.h"
#include "vm/thread.h"

namespace Impacto {

enum TextAlignment { TA_Left, TA_Center, TA_Right };

// TODO: think about / profile memory access patterns

struct DialogueColorPair {
  uint32_t TextColor;
  uint32_t OutlineColor;
};

struct ProcessedTextGlyph {
  DialogueColorPair Colors;
  Sprite Glyph;
  float Opacity;
  RectF DestRect;
};

enum DialoguePageMode : uint8_t { DPM_ADV = 0, DPM_NVL = 1 };

int const DialogueMaxNameLength = 32;
int const DialogueMaxRubyChunks = 32;
int const DialogueMaxRubyChunkLength = 32;

int const DialogueColors = 70;

struct DialoguePageFeatureConfig {
  RectF NVLBounds;
  RectF ADVBounds;
  TextAlignment ADVNameAlignment;
  float ADVNameFontSize;
  glm::vec2 ADVNamePos;
  float DefaultFontSize;
  float RubyFontSize;
  Font DialogueFont;
  DialogueColorPair ColorTable[DialogueColors];
};

extern DialoguePageFeatureConfig const DialoguePageFeatureConfig_RNE;

struct RubyChunk {
  int FirstBaseCharacter;
  int Length;
  uint16_t Text[DialogueMaxRubyChunkLength];
};

enum DialoguePageAnimState {
  DPAS_Hidden,
  DPAS_Hiding,
  DPAS_Showing,
  DPAS_Shown
};

struct DialoguePage {
  static int const MaxCharacters = 2000;

  Game* GameCtx;

  int Id;

  float ADVBoxOpacity;
  DialoguePageAnimState AnimState;

  int Length;
  int FullyOpaqueGlyphCount;
  bool TextIsFullyOpaque;

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
  void Update(float dt);
  void Render();
};

int TextGetStringLength(Vm::Sc3VmThread* ctx);

}  // namespace Impacto