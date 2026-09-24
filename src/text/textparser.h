#pragma once

#include "dialoguepage.h"
#include "backlogpage.h"
#include "tipspage.h"
#include <variant>

#include "../profile/dialogue.h"

namespace Impacto {

using namespace Profile::Dialogue;

enum class TextParserType { Dialogue, Backlog, Tips };

class TextParser {
 public:
  TextParser();
  virtual ~TextParser() = default;

  virtual void ParseString(Vm::Sc3VmThread* string) = 0;
  virtual constexpr TextParserType GetType() = 0;

  virtual void Reset();

  std::vector<ProcessedTextGlyph> Glyphs;
  std::vector<RubyChunk> RubyChunks;

  std::vector<ProcessedTextGlyph> Name;
  uint32_t NameId = NO_NAME;

  std::set<size_t> ParallelStartGlyphs;

  DialoguePage::AdvanceMethodType AdvanceMethod =
      DialoguePage::AdvanceMethodType::Skip;

 protected:
  template <StringTokenType T>
  void ParseStringToken(const StringToken& token);

  void FinishLine(size_t nextLineStart, bool force = false);
  void FinishName();

  std::variant<std::vector<uint16_t>, std::vector<uint32_t>> NameCode;

  enum class TextParsingState { Normal, Name, RubyBase, RubyAnnotation };
  TextParsingState ParsingState = TextParsingState::Normal;

  TextModeInfo ModeInfo = TextModesInfo[0];

  size_t LastLineStart = 0;

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
  constexpr TextParserType GetType() override {
    return TextParserType::Dialogue;
  }

  void ParseString(Vm::Sc3VmThread* string) override;
  void ParseString(DialoguePage& page, Vm::Sc3VmThread* string);
};

class BacklogTextParser : public TextParser {
 public:
  BacklogTextParser() = default;
  constexpr TextParserType GetType() override {
    return TextParserType::Backlog;
  }

  void ParseString(Vm::Sc3VmThread* string) override;
  void ParseString(BacklogPage& page, Vm::Sc3VmThread* string);
};

class TipsTextParser : public TextParser {
 public:
  TipsTextParser() = default;
  constexpr TextParserType GetType() override { return TextParserType::Tips; }

  void ParseString(Vm::Sc3VmThread* string) override;
  void ParseString(TipsPage& page, Vm::Sc3VmThread* string);
};

inline DialogueTextParser* DialogueTextParserInst = nullptr;
inline BacklogTextParser* BacklogTextParserInst = nullptr;
inline TipsTextParser* TipsTextParserInst = nullptr;

}  // namespace Impacto
