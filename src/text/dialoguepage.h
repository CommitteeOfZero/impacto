#pragma once

#include "text.h"
#include "typewritereffect.h"

#include "../audio/audiostream.h"

namespace Impacto {
enum DialoguePageMode : uint8_t {
  DPM_ADV = 0,
  DPM_NVL = 1,
  DPM_REV = 2,
  DPM_TIPS = 3
};

struct DialoguePage {
  static void Init();

  int Id = 0;
  int AnimationId = 0;
  int NextAnimationId = 0;
  int CharacterId = -1;

  std::array<RubyChunk, 32> RubyChunks;
  std::vector<ProcessedTextGlyph> Glyphs;

  TypewriterEffect Typewriter;
  Animation FadeAnimation;
  Animation TextFadeAnimation;

  size_t NameLength;

  std::vector<ProcessedTextGlyph> Name;

  RectF BoxBounds;

  Audio::AudioStream* CurrentVoice;

  glm::vec2 Dimensions;
  int Length;
  float FontSize;

  size_t RubyChunkCount;
  int CurrentRubyChunk;

  std::optional<uint32_t> NameId = std::nullopt;
  std::optional<uint32_t> PrevNameId = std::nullopt;
  bool RenderName = false;

  enum class AdvanceMethodType : uint8_t {
    Skip,
    Present,
    PresentClear,
    Present0x18,
    AutoForwardSyncVoice,
    AutoForward,
  };
  AdvanceMethodType AdvanceMethod = AdvanceMethodType::Skip;

  float AutoWaitTime = 0.0f;

  TextAlignment Alignment = TextAlignment::Left;
  DialoguePageMode Mode;

  bool CurrentLineVoiced = false;

  enum class State { Initial, Showing, Hiding, Shown, Hidden };
  State GetState() const;

  // TODO get rid of this
  bool TextIsFullyOpaque();
  void Clear();
  void ClearName();
  void AddString(Vm::Sc3VmThread* ctx, Audio::AudioStream* voice = 0,
                 bool acted = true, int animId = 0, int charId = -1,
                 bool shouldUpdateCharId = false);
  void Update(float dt);
  void Move(glm::vec2 relativePos);
  void MoveTo(glm::vec2 pos);
  void Render();
  void Hide();
  void Show();
  bool HasName() { return this->NameId.has_value(); }

 private:
  void FinishLine(Vm::Sc3VmThread* ctx, size_t nextLineStart,
                  const RectF& boxBounds, TextAlignment alignment);
  void EndRubyBase(int lastBaseCharacter);

  bool ShouldShowNewText = false;
  DialoguePageMode PrevMode = DPM_ADV;
  bool ShouldUpdateCharId = false;

  bool BuildingRubyBase;
  size_t FirstRubyChunkOnLine;

  size_t LastLineStart;
  float CurrentLineTop;
  float CurrentLineTopMargin;
};

inline std::vector<DialoguePage> DialoguePages;
inline bool SkipModeEnabled = false;
inline bool AutoModeEnabled = false;

}  // namespace Impacto