#include "textparser.h"

#include <numeric>
#include <magic_enum/magic_enum_containers.hpp>

#include "../profile/dialogue.h"
#include "../profile/games/chlcc/dialoguebox.h"

#include "../data/tipssystem.h"
#include "../hud/tipsnotification.h"

namespace Impacto {

using namespace Profile::Dialogue;
using namespace Profile::ScriptVars;

void TextParser::Reset() {
  Glyphs.clear();
  RubyChunks.clear();

  Name.clear();
  NameCode.clear();
  NameId = NO_NAME;

  AdvanceMethod = DialoguePage::AdvanceMethodType::Skip;
  ParsingState = TextParsingState::Normal;

  ParallelStartGlyphs.clear();

  LastLineStart = 0;

  FontSize = ModeInfo.TextGlyphSize.y;

  Alignment = TextAlignment::Left;
  CurrentX = 0.0f;
  CurrentLineTop = 0.0f;
  CurrentLineTopMargin = 0.0f;

  CurrentColors = ColorTable[0];
}

template <>
void TextParser::ParseStringToken<STT_LineBreak>(const StringToken& token) {
  FinishLine(Glyphs.size(), true);
  CurrentX = 0.0f;
}

template <>
void TextParser::ParseStringToken<STT_CharacterNameStart>(
    const StringToken& token) {
  NameCode.reserve(64);
  ParsingState = TextParsingState::Name;
}

template <>
void TextParser::ParseStringToken<STT_DialogueLineStart>(
    const StringToken& token) {
  if (ParsingState == TextParsingState::Name) FinishName();

  ParsingState = TextParsingState::Normal;
}

template <>
void TextParser::ParseStringToken<STT_Present>(const StringToken& token) {
  AdvanceMethod = DialoguePage::AdvanceMethodType::Present;
}

template <>
void TextParser::ParseStringToken<STT_SetColor>(const StringToken& token) {
  CurrentColors = ColorTable[token.Val_Expr];
}

template <>
void TextParser::ParseStringToken<STT_Present_Clear>(const StringToken& token) {
  AdvanceMethod = DialoguePage::AdvanceMethodType::PresentClear;
}

template <>
void TextParser::ParseStringToken<STT_RubyBaseStart>(const StringToken& token) {
  RubyChunks.emplace_back().FirstBaseCharacter = Glyphs.size();
  ParsingState = TextParsingState::RubyBase;
}

template <>
void TextParser::ParseStringToken<STT_RubyTextStart>(const StringToken& token) {
  assert(ParsingState == TextParsingState::RubyBase);
  RubyChunks.back().FinishBase(Glyphs.size());
  ParsingState = TextParsingState::RubyAnnotation;
}

template <>
void TextParser::ParseStringToken<STT_RubyTextEnd>(const StringToken& token) {
  // At least S;G uses [ruby-base]link text[ruby-text-end] for mails,
  // with no ruby-text-start
  if (ParsingState == TextParsingState::RubyBase) {
    RubyChunks.back().FinishBase(Glyphs.size());
  }

  ParsingState = TextParsingState::Normal;
}

template <>
void TextParser::ParseStringToken<STT_SetFontSize>(const StringToken& token) {
  FontSize = ModeInfo.TextGlyphSize.y * (token.Val_Uint16 / SetFontSizeRatio);
}

template <>
void TextParser::ParseStringToken<STT_PrintInParallel>(
    const StringToken& token) {
  ParallelStartGlyphs.insert(Glyphs.size());
}

template <>
void TextParser::ParseStringToken<STT_CenterText>(const StringToken& token) {
  Alignment = TextAlignment::Center;
}

template <>
void TextParser::ParseStringToken<STT_SetTopMargin>(const StringToken& token) {
  CurrentLineTopMargin = token.Val_Uint16;
}

template <>
void TextParser::ParseStringToken<STT_SetLeftMargin>(const StringToken& token) {
  float addX = token.Val_Uint16;
  if (CurrentX + addX > ModeInfo.MaxLineWidth) {
    FinishLine(Glyphs.size());
    addX -= (ModeInfo.MaxLineWidth - CurrentX);
    CurrentX = 0.0f;
  }
  while (addX > ModeInfo.MaxLineWidth) {
    FinishLine(Glyphs.size());
    addX -= ModeInfo.MaxLineWidth;
  }
  CurrentX += addX;
}

template <>
void TextParser::ParseStringToken<STT_UnlockTip>(const StringToken& token) {
  if (!TipsSystem::GetTipLockedState(token.Val_Uint16)) return;

  TipsSystem::SetTipLockedState(token.Val_Uint16, false);
  TipsNotification::AddTip(token.Val_Uint16);
  TipsSystem::GetNewTipsIndices().push_back(token.Val_Uint16);
}

template <>
void TextParser::ParseStringToken<STT_Present_0x18>(const StringToken& token) {
  AdvanceMethod = DialoguePage::AdvanceMethodType::Present0x18;
}

template <>
void TextParser::ParseStringToken<STT_AutoForward_SyncVoice>(
    const StringToken& token) {
  AdvanceMethod = DialoguePage::AdvanceMethodType::AutoForwardSyncVoice;
}

template <>
void TextParser::ParseStringToken<STT_AutoForward>(const StringToken& token) {
  AdvanceMethod = DialoguePage::AdvanceMethodType::AutoForward;
}

template <>
void TextParser::ParseStringToken<STT_RubyCenterPerCharacter>(
    const StringToken& token) {
  RubyChunks.back().CenterPerCharacter = true;
}

template <>
void TextParser::ParseStringToken<STT_Character>(const StringToken& token) {
  switch (ParsingState) {
    case TextParsingState::Name: {
      NameCode.emplace_back(SDL_Swap16(token.Val_Uint16 | 0x8000));
      return;
    }

    case TextParsingState::RubyAnnotation: {
      RubyChunks.back().RawText.push_back(
          SDL_Swap16(token.Val_Uint16 | 0x8000));
      return;
    }

    case TextParsingState::Normal:
    case TextParsingState::RubyBase: {
      // TODO respect TA_Center
      // TODO what to do about left margin if text alignment is center?
      ProcessedTextGlyph& glyph = Glyphs.emplace_back();
      glyph.CharId = token.Val_Uint16;

      glyph.Opacity = 1.0f;
      glyph.Colors = CurrentColors;

      glyph.DestRect.X = ModeInfo.WindowPos.x + CurrentX;
      glyph.DestRect.Width = (FontSize / DialogueFont->BitmapEmWidth) *
                             DialogueFont->AdvanceWidths[glyph.CharId];
      glyph.DestRect.Height = FontSize;

      CurrentX += glyph.DestRect.Width;

      // Line breaking
      if (CurrentX > ModeInfo.MaxLineWidth) {
        size_t breakCharacter = Glyphs.size() - 1;
        for (; breakCharacter > LastLineStart; breakCharacter--) {
          constexpr uint8_t dontBreakBeforeFlags =
              +CharacterTypeFlags::WordStartingPunct;
          constexpr uint8_t dontBreakOnFlags =
              +CharacterTypeFlags::WordEndingPunct |
              +CharacterTypeFlags::Alphabet;

          const bool prevCharacterAllowsBreak =
              breakCharacter < 1 ||
              !(StringToken::GetFlags(Glyphs[breakCharacter - 1].CharId) &
                dontBreakBeforeFlags);
          const bool curCharacterAllowsBreak =
              !(StringToken::GetFlags(Glyphs[breakCharacter].CharId) &
                dontBreakOnFlags);

          if (prevCharacterAllowsBreak && curCharacterAllowsBreak) {
            break;
          }
        }

        FinishLine(breakCharacter);

        // Skip all leading whitespace on the new line
        for (; breakCharacter < Glyphs.size(); breakCharacter++) {
          if (!(StringToken::GetFlags(Glyphs[breakCharacter].CharId) &
                +CharacterTypeFlags::Space)) {
            break;
          }
        }

        CurrentX = 0.0f;
        for (size_t i = breakCharacter; i < Glyphs.size(); i++) {
          Glyphs[i].DestRect.X = ModeInfo.WindowPos.x + CurrentX;
          CurrentX += Glyphs[i].DestRect.Width;
        }
      }

      return;
    }
  }
}

void TextParser::FinishLine(const size_t nextLineStart, const bool force) {
  if (!force && nextLineStart == LastLineStart) return;

  const std::span<ProcessedTextGlyph> currentLine =
      std::span(Glyphs.begin() + LastLineStart, nextLineStart - LastLineStart);

  // Lay out all ruby chunks on this line (before we change CurrentLineTop and
  // thus can't find where to put them)
  bool hasRuby = false;
  const float rubyFontSize =
      ModeInfo.RubyGlyphSize.y * FontSize / DefaultFontSize;
  for (RubyChunk& chunk : RubyChunks) {
    const size_t chunkSize = chunk.RawText.size();

    if (chunkSize == 0 || chunk.BaseLength == 0 ||
        chunk.FirstBaseCharacter < LastLineStart) {
      continue;
    }
    if (chunk.FirstBaseCharacter >= nextLineStart) break;
    hasRuby = true;

    Vm::Sc3Stream rubyText(chunk.RawText.data());

    const std::span<const ProcessedTextGlyph> base =
        std::span(Glyphs.begin() + chunk.FirstBaseCharacter, chunk.BaseLength);
    glm::vec2 pos =
        glm::vec2(base.front().DestRect.X,
                  ModeInfo.WindowPos.y + CurrentLineTop + CurrentLineTopMargin +
                      (ModeInfo.RubyGlyphSize.y - rubyFontSize));

    // ruby base length > ruby text length: block align
    // ruby base length > ruby text length and 0x1E: center per character
    // ruby base length == ruby text length: center per character
    // ruby base length < ruby text length: center over block (handled by
    // block align)

    chunk.Text.resize(chunkSize);
    if (chunkSize == chunk.BaseLength ||
        (chunk.CenterPerCharacter && chunk.BaseLength > chunkSize)) {
      // center every ruby character over the base character below it
      for (size_t j = 0; j < chunkSize; j++) {
        pos.x = base[j].DestRect.Center().x;
        TextLayoutPlainLine(rubyText, 1, std::span(chunk.Text.begin() + j, 1),
                            DialogueFont, rubyFontSize, CurrentColors, 1.0f,
                            pos, TextAlignment::Center);
      }
    } else {
      TextLayoutPlainLine(rubyText, static_cast<int>(chunkSize), chunk.Text,
                          DialogueFont, rubyFontSize, CurrentColors, 1.0f, pos,
                          TextAlignment::Left);
      const float baseWidth =
          base.back().DestRect.Right() - base.front().DestRect.X;
      const float nonSpacedWidth =
          chunk.Text.back().DestRect.Right() - chunk.Text.front().DestRect.X;
      const float excessWidth = baseWidth - nonSpacedWidth;

      if (chunkSize == 1) {
        chunk.Text.front().DestRect.X += baseWidth / 2.0f;
      } else if (excessWidth <= 0.0f) {
        // Ruby overflows => center over base with normal spacing
        const float offsetX = (baseWidth - nonSpacedWidth) / 2.0f;
        for (auto& glyph : chunk.Text) {
          glyph.DestRect.X += offsetX;
        }
      } else {
        // Evenly space out all ruby characters over the block of base text
        const float extraSpacing = excessWidth / (chunkSize - 1);
        for (size_t rubyGlyphId = 0; rubyGlyphId < chunkSize; rubyGlyphId++) {
          chunk.Text[rubyGlyphId].DestRect.X += extraSpacing * rubyGlyphId;
        }
      }

      // TODO is this really the right behaviour for
      // CenterPerCharacter(0x1E) and ruby base length < ruby text length?
    }
  }
  // Despite the font size actually being rubyFontSize, the line displacement is
  // still handled by ModeInfo.RubyGlyphSize.y
  if (ModeInfo.AlwaysAddRubySpacing || hasRuby) {
    const float rubyHeight = hasRuby ? rubyFontSize : ModeInfo.RubyGlyphSize.y;
    CurrentLineTop += ModeInfo.RubyLineSpacing + rubyHeight;

    // Align name height with the first line of text if NameDispMode is
    // RelativeToWindow MAGES. engine also hardcodes it to only happen for the
    // backlog
    if (GetType() == TextParserType::Backlog && hasRuby && LastLineStart == 0 &&
        ModeInfo.NameDispMode ==
            TextModeInfo::NameDispModeType::RelativeToWindow) {
      for (auto& glyph : Name) {
        glyph.DestRect.Y += ModeInfo.RubyLineSpacing + rubyHeight;
      }
    }
  }

  // Glyphs of different font sizes are bottom-aligned within the line
  const float lineHeight =
      std::accumulate(currentLine.begin(), currentLine.end(), FontSize,
                      [](float lhs, const auto& rhs) {
                        return std::max(lhs, rhs.DestRect.Height);
                      });

  // completely trial and error guess
  const float normalizedFontSize = FontSize / ModeInfo.TextGlyphSize.y;
  CurrentLineTopMargin *= normalizedFontSize;

  if (!currentLine.empty()) {
    float marginXOffset = 0;
    if (currentLine.front().DestRect.X > ModeInfo.WindowPos.x) {
      marginXOffset =
          (currentLine.front().DestRect.Right() - ModeInfo.WindowPos.x) *
          (normalizedFontSize - 1.0f);
    }

    const float lastGlyphX = currentLine.back().DestRect.Right();
    const float xAlignmentOffset = [&]() {
      switch (Alignment) {
        case TextAlignment::Center:
          return (ModeInfo.MaxLineWidth - (lastGlyphX - ModeInfo.WindowPos.x)) /
                 2.0f;
          break;
        case TextAlignment::Right:
          return ModeInfo.MaxLineWidth - lastGlyphX - marginXOffset;
          break;
        case TextAlignment::Left:
          return marginXOffset;
        default:
          return 0.0f;
      }
    }();
    for (ProcessedTextGlyph& glyph : currentLine) {
      glyph.DestRect.X += xAlignmentOffset;
      glyph.DestRect.Y = ModeInfo.WindowPos.y + CurrentLineTop +
                         CurrentLineTopMargin +
                         (lineHeight - glyph.DestRect.Height);
    }
  }

  CurrentLineTop += CurrentLineTopMargin + lineHeight + ModeInfo.LineSpacing;
  CurrentLineTopMargin = 0.0f;

  LastLineStart = nextLineStart;
}

void TextParser::FinishName() {
  using enum TextModeInfo::NameDispModeType;
  using enum TextModeInfo::NameAlignmentType;

  if (ModeInfo.NameDispMode == Invisible || ModeInfo.MaxNameWidth == 0.0f ||
      NameCode.empty()) {
    return;
  }

  Vm::Sc3Stream nameStream(NameCode.data());
  const float nameWidth = TextGetPlainLineWidth(
      nameStream, DialogueFont, ModeInfo.NameGlyphSize.y, NameCode.size());

  glm::vec2 pos{};
  switch (ModeInfo.NameDispMode) {
    case RelativeToWindow:
      pos = ModeInfo.WindowPos + ModeInfo.NamePos;

      if (OldNametagPositioning) {
        switch (ModeInfo.NameAlignment) {
          case Left:
            break;
          case Center:
            pos.x += (ModeInfo.MaxNameWidth - nameWidth) / 2.0f;
            break;
          case Right:
            pos.x += ModeInfo.MaxNameWidth - nameWidth;
            break;
        }
      } else {
        switch (ModeInfo.NameAlignment) {
          case Left:
            break;
          case Center:
            pos.x -= nameWidth / 2.0f;
            break;
          case Right:
            pos.x += ModeInfo.MaxNameWidth - nameWidth;
            break;
        }
      }
      break;

    case InText:
      pos = ModeInfo.WindowPos +
            glm::vec2(CurrentX, CurrentLineTop + CurrentLineTopMargin);
      break;

    case FixedPos:
      pos = ModeInfo.NamePos + glm::vec2(ModeInfo.MaxNameWidth, 0.0f);

      if (OldNametagPositioning) {
        switch (ModeInfo.NameAlignment) {
          case Left:
            break;
          case Center:
            pos.x -= nameWidth / 2.0f;
            break;
          case Right:
            pos.x += nameWidth;
            break;
        }
      } else {
        switch (ModeInfo.NameAlignment) {
          case Left:
            pos.x -= ModeInfo.MaxNameWidth;
            break;
          case Center:
            pos.x -= nameWidth / 2.0f;
            break;
          case Right:
            pos.x += ModeInfo.NamePos.x + ModeInfo.MaxNameWidth - nameWidth;
            break;
        }
      }
      break;

    case Invisible:
      assert(false);
      break;
  }

  nameStream = Vm::Sc3Stream(NameCode.data());
  Name = TextLayoutPlainLine(nameStream, static_cast<int>(NameCode.size()),
                             DialogueFont, ModeInfo.NameGlyphSize.y,
                             ColorTable[0], 1.0f, pos, TextAlignment::Left);
  assert(NameCode.size() == Name.size());

  if (ModeInfo.NameDispMode == InText) {
    CurrentLineTop += ModeInfo.NameGlyphSize.y + ModeInfo.LineSpacing;
  }
}

void DialogueTextParser::ParseString(Vm::Sc3VmThread* string) {
  using TokenParserProc = auto (TextParser::*)(const StringToken&)->void;
  constexpr static auto tokenParserLUT = []() {
    magic_enum::containers::array<StringTokenType, TokenParserProc> lut;
    lut.fill(nullptr);

    lut[STT_LineBreak] = &DialogueTextParser::ParseStringToken<STT_LineBreak>;
    lut[STT_CharacterNameStart] =
        &DialogueTextParser::ParseStringToken<STT_CharacterNameStart>;
    lut[STT_DialogueLineStart] =
        &DialogueTextParser::ParseStringToken<STT_DialogueLineStart>;
    lut[STT_Present] = &DialogueTextParser::ParseStringToken<STT_Present>;
    lut[STT_SetColor] = &DialogueTextParser::ParseStringToken<STT_SetColor>;
    lut[STT_Present_Clear] =
        &DialogueTextParser::ParseStringToken<STT_Present_Clear>;
    lut[STT_RubyBaseStart] =
        &DialogueTextParser::ParseStringToken<STT_RubyBaseStart>;
    lut[STT_RubyTextStart] =
        &DialogueTextParser::ParseStringToken<STT_RubyTextStart>;
    lut[STT_RubyTextEnd] =
        &DialogueTextParser::ParseStringToken<STT_RubyTextEnd>;
    lut[STT_SetFontSize] =
        &DialogueTextParser::ParseStringToken<STT_SetFontSize>;
    lut[STT_PrintInParallel] =
        &DialogueTextParser::ParseStringToken<STT_PrintInParallel>;
    lut[STT_CenterText] = &DialogueTextParser::ParseStringToken<STT_CenterText>;
    lut[STT_SetTopMargin] =
        &DialogueTextParser::ParseStringToken<STT_SetTopMargin>;
    lut[STT_SetLeftMargin] =
        &DialogueTextParser::ParseStringToken<STT_SetLeftMargin>;
    lut[STT_UnlockTip] = &DialogueTextParser::ParseStringToken<STT_UnlockTip>;
    lut[STT_Present_0x18] =
        &DialogueTextParser::ParseStringToken<STT_Present_0x18>;
    lut[STT_AutoForward_SyncVoice] =
        &DialogueTextParser::ParseStringToken<STT_AutoForward_SyncVoice>;
    lut[STT_AutoForward] =
        &DialogueTextParser::ParseStringToken<STT_AutoForward>;
    lut[STT_RubyCenterPerCharacter] =
        &DialogueTextParser::ParseStringToken<STT_RubyCenterPerCharacter>;
    lut[STT_AltLineBreak] =
        &DialogueTextParser::ParseStringToken<STT_LineBreak>;
    lut[STT_Character] = &DialogueTextParser::ParseStringToken<STT_Character>;

    return lut;
  }();

  const size_t glyphsStart = Glyphs.size();
  const size_t rubyChunksStart = RubyChunks.size();

  StringToken token;
  do {
    token.Read(string);
    const TokenParserProc parser = tokenParserLUT[token.Type];
    if (parser != nullptr) std::invoke(parser, this, token);
  } while (token.Type != STT_EndOfString);

  FinishLine(Glyphs.size());

  NameId = NameCode.empty()
               ? NO_NAME
               : static_cast<uint16_t>(GetNameId(NameCode).value_or(NO_NAME));

  for (size_t glyphIdx = glyphsStart; glyphIdx < Glyphs.size(); glyphIdx++) {
    Glyphs[glyphIdx].Opacity = 0.0f;
  }
  for (size_t chunkIdx = rubyChunksStart; chunkIdx < RubyChunks.size();
       chunkIdx++) {
    for (auto& glyph : RubyChunks[chunkIdx].Text) {
      glyph.Opacity = 0.0f;
    }
  }
}

void DialogueTextParser::ParseString(DialoguePage& page,
                                     Vm::Sc3VmThread* string) {
  Reset();
  Glyphs.swap(page.Glyphs);
  RubyChunks.swap(page.RubyChunks);
  Name.swap(page.Name);
  CurrentLineTop = page.CurrentLineTop;
  CurrentLineTopMargin = page.CurrentLineTopMargin;
  LastLineStart = Glyphs.size();

  ModeInfo = page.GetTextModeInfo();
  FontSize = ModeInfo.TextGlyphSize.y;

  ParseString(string);

  ScrWork[SW_MESNAMEID0 + page.Id] = DialogueTextParserInst.NameId;

  Glyphs.swap(page.Glyphs);
  RubyChunks.swap(page.RubyChunks);
  Name.swap(page.Name);
  page.AdvanceMethod = AdvanceMethod;
  page.CurrentLineTop = CurrentLineTop;
  page.CurrentLineTopMargin = CurrentLineTopMargin;
}

void BacklogTextParser::ParseString(Vm::Sc3VmThread* string) {
  using TokenParserProc = auto (TextParser::*)(const StringToken&)->void;
  constexpr static auto tokenParserLUT = []() {
    magic_enum::containers::array<StringTokenType, TokenParserProc> lut;
    lut.fill(nullptr);

    lut[STT_LineBreak] = &BacklogTextParser::ParseStringToken<STT_LineBreak>;
    lut[STT_CharacterNameStart] =
        &BacklogTextParser::ParseStringToken<STT_CharacterNameStart>;
    lut[STT_DialogueLineStart] =
        &BacklogTextParser::ParseStringToken<STT_DialogueLineStart>;
    lut[STT_RubyBaseStart] =
        &BacklogTextParser::ParseStringToken<STT_RubyBaseStart>;
    lut[STT_RubyTextStart] =
        &BacklogTextParser::ParseStringToken<STT_RubyTextStart>;
    lut[STT_RubyTextEnd] =
        &BacklogTextParser::ParseStringToken<STT_RubyTextEnd>;
    lut[STT_CenterText] = &BacklogTextParser::ParseStringToken<STT_CenterText>;
    lut[STT_RubyCenterPerCharacter] =
        &BacklogTextParser::ParseStringToken<STT_RubyCenterPerCharacter>;
    lut[STT_Character] = &BacklogTextParser::ParseStringToken<STT_Character>;

    return lut;
  }();

  CurrentColors = ColorTable[REVColor];

  StringToken token;
  do {
    token.Read(string);
    const TokenParserProc parser = tokenParserLUT[token.Type];
    if (parser != nullptr) std::invoke(parser, this, token);
  } while (token.Type != STT_EndOfString);

  FinishLine(Glyphs.size());
}

void BacklogTextParser::ParseString(BacklogPage& page,
                                    Vm::Sc3VmThread* string) {
  Reset();
  Glyphs.swap(page.Glyphs);
  RubyChunks.swap(page.RubyChunks);
  Name.swap(page.Name);
  LastLineStart = Glyphs.size();

  ModeInfo = TextModesInfo[REVMessageModeIdx];
  FontSize = ModeInfo.TextGlyphSize.y;

  ParseString(string);

  for (ProcessedTextGlyph& glyph : Name) {
    glyph.Colors = ColorTable[REVNameColor];
  }

  Glyphs.swap(page.Glyphs);
  RubyChunks.swap(page.RubyChunks);
  Name.swap(page.Name);
}

void TipsTextParser::ParseString(Vm::Sc3VmThread* string) {
  using TokenParserProc = auto (TextParser::*)(const StringToken&)->void;
  constexpr static auto tokenParserLUT = []() {
    magic_enum::containers::array<StringTokenType, TokenParserProc> lut;
    lut.fill(nullptr);

    lut[STT_LineBreak] = &TipsTextParser::ParseStringToken<STT_LineBreak>;
    lut[STT_SetColor] = &TipsTextParser::ParseStringToken<STT_SetColor>;
    lut[STT_RubyBaseStart] =
        &TipsTextParser::ParseStringToken<STT_RubyBaseStart>;
    lut[STT_RubyTextStart] =
        &TipsTextParser::ParseStringToken<STT_RubyTextStart>;
    lut[STT_RubyTextEnd] = &TipsTextParser::ParseStringToken<STT_RubyTextEnd>;
    lut[STT_SetFontSize] = &TipsTextParser::ParseStringToken<STT_SetFontSize>;
    lut[STT_CenterText] = &TipsTextParser::ParseStringToken<STT_CenterText>;
    lut[STT_SetTopMargin] = &TipsTextParser::ParseStringToken<STT_SetTopMargin>;
    lut[STT_SetLeftMargin] =
        &TipsTextParser::ParseStringToken<STT_SetLeftMargin>;
    lut[STT_RubyCenterPerCharacter] =
        &TipsTextParser::ParseStringToken<STT_RubyCenterPerCharacter>;
    lut[STT_AltLineBreak] = &TipsTextParser::ParseStringToken<STT_LineBreak>;
    lut[STT_Character] = &TipsTextParser::ParseStringToken<STT_Character>;

    return lut;
  }();

  CurrentColors = ColorTable[TipsColorIndex];

  StringToken token;
  do {
    token.Read(string);
    const TokenParserProc parser = tokenParserLUT[token.Type];
    if (parser != nullptr) std::invoke(parser, this, token);
  } while (token.Type != STT_EndOfString);

  FinishLine(Glyphs.size());
}

void TipsTextParser::ParseString(TipsPage& page, Vm::Sc3VmThread* string) {
  Reset();
  Glyphs.swap(page.Glyphs);
  RubyChunks.swap(page.RubyChunks);

  ModeInfo = TextModesInfo[TipsMessageModeIdx];
  FontSize = ModeInfo.TextGlyphSize.y;

  ParseString(string);

  Glyphs.swap(page.Glyphs);
  RubyChunks.swap(page.RubyChunks);
}

}  // namespace Impacto
