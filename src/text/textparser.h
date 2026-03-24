#pragma once

#include "dialoguepage.h"

#include "../profile/dialogue.h"

namespace Impacto {

using namespace Profile::Dialogue;

class TextParser {
 public:
  virtual ~TextParser() = default;

  virtual void ParseString(Vm::Sc3VmThread* string) = 0;

  virtual void Reset();

  std::vector<ProcessedTextGlyph> Glyphs;
  std::vector<RubyChunk> RubyChunks;

  std::vector<ProcessedTextGlyph> Name;
  uint16_t NameId = 0xFFFF;

  std::set<size_t> ParallelStartGlyphs;

  DialoguePage::AdvanceMethodType AdvanceMethod =
      DialoguePage::AdvanceMethodType::Skip;

  DialoguePageMode PageMode = DPM_ADV;

 protected:
  void ParseLineBreak(const StringToken& token);
  void ParseCharacterNameStart(const StringToken& token);
  void ParseDialogueLineStart(const StringToken& token);
  void ParsePresent(const StringToken& token);
  void ParseSetColor(const StringToken& token);
  void ParsePresentClear(const StringToken& token);
  void ParseRubyBaseStart(const StringToken& token);
  void ParseRubyTextStart(const StringToken& token);
  void ParseRubyTextEnd(const StringToken& token);
  void ParseSetFontSize(const StringToken& token);
  void ParsePrintInParallel(const StringToken& token);
  void ParseCenterText(const StringToken& token);
  void ParseSetTopMargin(const StringToken& token);
  void ParseSetLeftMargin(const StringToken& token);
  void ParseUnlockTip(const StringToken& token);
  void ParsePreset0x18(const StringToken& token);
  void ParseAutoForwardSyncVoice(const StringToken& token);
  void ParseAutoForward(const StringToken& token);
  void ParseRubyCenterPerCharacter(const StringToken& token);
  void ParseCharacter(const StringToken& token);

  void FinishLine(size_t nextLineStart);
  void EndRubyBase();

  std::vector<uint16_t> NameCode;

  enum class TextParsingState { Normal, Name, Ruby };
  TextParsingState ParsingState = TextParsingState::Normal;

  bool BuildingRubyBase = false;
  size_t LastRubyBaseCharacter = 0;
  size_t FirstRubyChunkOnLine = 0;

  RectF BoxBounds;

  size_t LastLineStart = 0;
  size_t LastWordStart = 0;
  bool PrevGlyphWordStarting = false;
  bool PrevGlyphWordEnding = false;

  float FontSize = DefaultFontSize;

  TextAlignment Alignment = TextAlignment::Left;
  float CurrentX = 0.0f;
  float CurrentLineTop = 0.0f;
  float CurrentLineTopMargin = 0.0f;

  DialogueColorPair CurrentColors;
};

class DialogueTextParser : public TextParser {
 public:
  DialogueTextParser() = default;

  void ParseString(Vm::Sc3VmThread* string) override;
  void ParseString(DialoguePage& page, Vm::Sc3VmThread* string);
};

inline DialogueTextParser DialogueTextParserInst;

}  // namespace Impacto
