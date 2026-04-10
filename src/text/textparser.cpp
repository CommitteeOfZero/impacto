#include "textparser.h"

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
  NameId = 0xFFFF;

  AdvanceMethod = DialoguePage::AdvanceMethodType::Skip;
  ParsingState = TextParsingState::Normal;

  BuildingRubyBase = false;
  FirstRubyChunkOnLine = 0;

  ParallelStartGlyphs.clear();

  PageMode = DPM_ADV;
  BoxBounds = ADVBounds;

  LastLineStart = 0;

  FontSize = DefaultFontSize;

  Alignment = TextAlignment::Left;
  CurrentX = 0.0f;
  CurrentLineTop = 0.0f;
  CurrentLineTopMargin = 0.0f;

  CurrentColors = ColorTable[0];
}

template <>
void TextParser::ParseStringToken<STT_LineBreak>(const StringToken& token) {
  FinishLine(Glyphs.size());
  CurrentX = 0.0f;
}

template <>
void TextParser::ParseStringToken<STT_CharacterNameStart>(
    const StringToken& token) {
  NameCode.reserve(64);
  ParsingState = TextParsingState::Name;
  if (PageMode == DPM_REV && REVNameLocation != REVNameLocationType::LeftTop) {
    CurrentLineTop += REVNameOffset;
  }
}

template <>
void TextParser::ParseStringToken<STT_DialogueLineStart>(
    const StringToken& token) {
  ParsingState = TextParsingState::Normal;
}

template <>
void TextParser::ParseStringToken<STT_Present>(const StringToken& token) {
  AdvanceMethod = DialoguePage::AdvanceMethodType::Present;
}

template <>
void TextParser::ParseStringToken<STT_SetColor>(const StringToken& token) {
  if (PageMode == DPM_REV) return;

  assert(token.Val_Expr < ColorCount);
  CurrentColors = ColorTable[token.Val_Expr];
}

template <>
void TextParser::ParseStringToken<STT_Present_Clear>(const StringToken& token) {
  AdvanceMethod = DialoguePage::AdvanceMethodType::PresentClear;
}

template <>
void TextParser::ParseStringToken<STT_RubyBaseStart>(const StringToken& token) {
  RubyChunks.emplace_back().FirstBaseCharacter = Glyphs.size();
  BuildingRubyBase = true;
}

template <>
void TextParser::ParseStringToken<STT_RubyTextStart>(const StringToken& token) {
  EndRubyBase();
  ParsingState = TextParsingState::Ruby;
}

template <>
void TextParser::ParseStringToken<STT_RubyTextEnd>(const StringToken& token) {
  // At least S;G uses [ruby-base]link text[ruby-text-end] for mails,
  // with no ruby-text-start
  EndRubyBase();
  ParsingState = TextParsingState::Normal;
}

template <>
void TextParser::ParseStringToken<STT_SetFontSize>(const StringToken& token) {
  FontSize = DefaultFontSize * (token.Val_Uint16 / SetFontSizeRatio);
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
  if (CurrentX + addX > BoxBounds.Width) {
    FinishLine(Glyphs.size());
    addX -= (BoxBounds.Width - CurrentX);
    CurrentX = 0.0f;
  }
  while (addX > BoxBounds.Width) {
    FinishLine(Glyphs.size());
    addX -= BoxBounds.Width;
  }
  CurrentX += addX;
}

template <>
void TextParser::ParseStringToken<STT_UnlockTip>(const StringToken& token) {
  if ((PageMode == DPM_ADV ||
       (ScrWork[SW_MESWIN0TYPE] == 1 && PageMode == DPM_REV) ||
       PageMode == DPM_NVL) &&
      TipsSystem::GetTipLockedState(token.Val_Uint16)) {
    TipsSystem::SetTipLockedState(token.Val_Uint16, false);
    TipsNotification::AddTip(token.Val_Uint16);
    TipsSystem::GetNewTipsIndices().push_back(token.Val_Uint16);
  }
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

    case TextParsingState::Ruby: {
      RubyChunk& curChunk = RubyChunks.back();
      curChunk.RawText[curChunk.Length] = SDL_Swap16(token.Val_Uint16 | 0x8000);
      curChunk.Length++;
      return;
    }

    case TextParsingState::Normal: {
      // TODO respect TA_Center
      // TODO what to do about left margin if text alignment is center?
      ProcessedTextGlyph& glyph = Glyphs.emplace_back();
      glyph.CharId = token.Val_Uint16;

      glyph.Opacity =
          (PageMode == DPM_REV || PageMode == DPM_TIPS) ? 1.0f : 0.0f;
      glyph.Colors = CurrentColors;

      glyph.DestRect.X = BoxBounds.X + CurrentX;
      glyph.DestRect.Width = (FontSize / DialogueFont->BitmapEmWidth) *
                             DialogueFont->AdvanceWidths[glyph.CharId];
      glyph.DestRect.Height = FontSize;

      CurrentX += glyph.DestRect.Width;

      // Line breaking
      if (glyph.DestRect.Right() > BoxBounds.Right()) {
        size_t breakCharacter = Glyphs.size() - 1;
        for (; breakCharacter > LastLineStart; breakCharacter--) {
          constexpr uint8_t dontBreakBeforeFlags =
              +CharacterTypeFlags::WordStartingPunct;
          constexpr uint8_t dontBreakOnFlags =
              +CharacterTypeFlags::WordEndingPunct |
              +CharacterTypeFlags::Alphabet;
          if (!(StringToken::GetFlags(Glyphs[breakCharacter - 1].CharId) &
                dontBreakBeforeFlags) &&
              !(StringToken::GetFlags(Glyphs[breakCharacter].CharId) &
                dontBreakOnFlags)) {
            break;
          }
        }

        FinishLine(breakCharacter);

        for (; breakCharacter < Glyphs.size(); breakCharacter++) {
          if (!(StringToken::GetFlags(Glyphs[breakCharacter].CharId) &
                +CharacterTypeFlags::Space)) {
            break;
          }
        }

        CurrentX = 0.0f;
        for (size_t i = breakCharacter; i < Glyphs.size(); i++) {
          Glyphs[i].DestRect.X = BoxBounds.X + CurrentX;
          CurrentX += Glyphs[i].DestRect.Width;
        }
      }

      return;
    }
  }
}

void TextParser::FinishLine(const size_t nextLineStart) {
  // Lay out all ruby chunks on this line (before we change CurrentLineTop and
  // thus can't find where to put them)
  for (RubyChunk& chunk : RubyChunks) {
    if (chunk.FirstBaseCharacter >= nextLineStart) break;

    Vm::Sc3Stream rubyText(chunk.RawText.data());

    glm::vec2 pos =
        glm::vec2(Glyphs[chunk.FirstBaseCharacter].DestRect.X,
                  CurrentLineTop + CurrentLineTopMargin + RubyYOffset);

    // ruby base length > ruby text length: block align
    // ruby base length > ruby text length and 0x1E: center per character
    // ruby base length == ruby text length: center per character
    // ruby base length < ruby text length: center over block (handled by
    // block align)

    if (chunk.Length == chunk.BaseLength ||
        (chunk.CenterPerCharacter && chunk.BaseLength > chunk.Length)) {
      // center every ruby character over the base character below it
      for (size_t j = 0; j < chunk.Length; j++) {
        RectF const& baseGlyphRect =
            Glyphs[chunk.FirstBaseCharacter + j].DestRect;
        pos.x = baseGlyphRect.Center().x;
        TextLayoutPlainLine(rubyText, 1, std::span(chunk.Text.begin() + j, 1),
                            DialogueFont, RubyFontSize, ColorTable[0], 1.0f,
                            pos, TextAlignment::Center);
      }
    } else {
      TextLayoutPlainLine(rubyText, static_cast<int>(chunk.Length), chunk.Text,
                          DialogueFont, RubyFontSize, ColorTable[0], 1.0f, pos,
                          TextAlignment::Left);
      const float baseWidth =
          (Glyphs[chunk.FirstBaseCharacter + chunk.BaseLength - 1].DestRect.X +
           Glyphs[chunk.FirstBaseCharacter + chunk.BaseLength - 1]
               .DestRect.Width) -
          Glyphs[chunk.FirstBaseCharacter].DestRect.X;
      const float nonSpacedWidth =
          (chunk.Text[chunk.Length - 1].DestRect.X +
           chunk.Text[chunk.Length - 1].DestRect.Width) -
          chunk.Text[0].DestRect.X;
      const float excessWidth = baseWidth - nonSpacedWidth;

      if (chunk.Length == 1) {
        chunk.Text[0].DestRect.X += baseWidth / 2.0f;
      } else if (excessWidth <= 0.0f) {
        // Ruby overflows => center over base with normal spacing
        const float offsetX = (baseWidth - nonSpacedWidth) / 2.0f;
        for (size_t rubyGlyphId = 0; rubyGlyphId < chunk.Length;
             rubyGlyphId++) {
          chunk.Text[rubyGlyphId].DestRect.X += offsetX;
        }
      } else {
        // Evenly space out all ruby characters over the block of base text
        const float extraSpacing = excessWidth / (chunk.Length - 1);
        for (size_t rubyGlyphId = 0; rubyGlyphId < chunk.Length;
             rubyGlyphId++) {
          chunk.Text[rubyGlyphId].DestRect.X += extraSpacing * rubyGlyphId;
        }
      }

      // TODO is this really the right behaviour for
      // CenterPerCharacter(0x1E) and ruby base length < ruby text length?
    }
    FirstRubyChunkOnLine++;
  }

  float lineHeight = FontSize;
  // Glyphs of different font sizes are bottom-aligned within the line
  for (size_t i = LastLineStart; i < nextLineStart; i++) {
    if (Glyphs[i].DestRect.Height > lineHeight)
      lineHeight = Glyphs[i].DestRect.Height;
  }

  // completely trial and error guess
  if (CurrentLineTopMargin) {
    CurrentLineTopMargin *= (FontSize / DefaultFontSize);
  }
  float marginXOffset = 0;
  if (LastLineStart < nextLineStart &&
      Glyphs[LastLineStart].DestRect.X > BoxBounds.X) {
    marginXOffset = (Glyphs[LastLineStart].DestRect.X - BoxBounds.X) *
                    ((FontSize / DefaultFontSize) - 1.0f);
  }
  for (size_t i = LastLineStart; i < nextLineStart; i++) {
    Glyphs[i].DestRect.Y = CurrentLineTop + CurrentLineTopMargin +
                           (lineHeight - Glyphs[i].DestRect.Height);
    float lastGlyphX = Glyphs[nextLineStart - 1].DestRect.X +
                       Glyphs[nextLineStart - 1].DestRect.Width;
    switch (Alignment) {
      case TextAlignment::Center:
        Glyphs[i].DestRect.X +=
            (BoxBounds.Width - (lastGlyphX - BoxBounds.X)) / 2.0f;
        break;
      case TextAlignment::Right:
        Glyphs[i].DestRect.X += BoxBounds.Width - lastGlyphX - marginXOffset;
        break;
      case TextAlignment::Left:
        Glyphs[i].DestRect.X += marginXOffset;
      default:
        break;
    }
  }
  float lineSpacing = DialogueFont->LineSpacing;
  if (PageMode == DPM_TIPS) lineSpacing = TipsLineSpacing;
  CurrentLineTop =
      CurrentLineTop + CurrentLineTopMargin + lineHeight + lineSpacing;
  CurrentLineTopMargin = 0.0f;
  LastLineStart = nextLineStart;
}

void TextParser::EndRubyBase() {
  if (BuildingRubyBase && !RubyChunks.empty()) {
    RubyChunk& chunk = RubyChunks.back();
    chunk.BaseLength = Glyphs.size() - chunk.FirstBaseCharacter;
  }

  BuildingRubyBase = false;
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
    lut[STT_Present_0x18] = &DialogueTextParser::ParseStringToken<STT_Present>;
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

  switch (PageMode) {
    case DPM_ADV:
      BoxBounds = ADVBounds;
      break;
    case DPM_NVL:
      BoxBounds = NVLBounds;
      break;
    case DPM_REV:
      BoxBounds = SecondaryREVBounds;
      FontSize = Profile::CHLCC::DialogueBox::REVFontSize;
      break;
    case DPM_TIPS:
      BoxBounds = TipsBounds;
      CurrentColors = ColorTable[TipsColorIndex];
      break;
  }

  StringToken token;
  do {
    token.Read(string);
    const TokenParserProc parser = tokenParserLUT[token.Type];
    if (parser != nullptr) std::invoke(parser, this, token);
  } while (token.Type != STT_EndOfString);

  FinishLine(Glyphs.size());

  if (!NameCode.empty()) {
    NameId = static_cast<uint16_t>(GetNameId(NameCode).value_or(0xFFFF));

    float fontSize = ADVNameFontSize;
    glm::vec2 pos = ADVNamePos;
    TextAlignment alignment = ADVNameAlignment;
    int colorIndex = 0;
    if (PageMode == DPM_REV) {
      fontSize = REVNameFontSize;
      colorIndex = REVNameColor;

      switch (REVNameLocation) {
        case REVNameLocationType::None:
        case REVNameLocationType::TopLeft:
          pos = REVBounds.GetPos();
          alignment = TextAlignment::Left;
          break;
        case REVNameLocationType::LeftTop:
          pos = {REVBounds.X - REVNameOffset, Glyphs[0].DestRect.Y};
          alignment = TextAlignment::Right;
          break;
      }
    }

    Vm::Sc3Stream nameStream(NameCode.data());
    Name = TextLayoutPlainLine(nameStream, static_cast<int>(NameCode.size()),
                               DialogueFont, fontSize, ColorTable[colorIndex],
                               1.0f, pos, alignment);
    assert(NameCode.size() == Name.size());
  } else {
    NameId = 0xFFFF;
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
  PageMode = page.GetMode();

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

  FontSize = DefaultFontSize;
  BoxBounds = REVBounds;
  CurrentColors = ColorTable[REVColor];

  StringToken token;
  do {
    token.Read(string);
    const TokenParserProc parser = tokenParserLUT[token.Type];
    if (parser != nullptr) std::invoke(parser, this, token);
  } while (token.Type != STT_EndOfString);

  FinishLine(Glyphs.size());

  if (!NameCode.empty()) {
    float fontSize = REVNameFontSize;
    int colorIndex = REVNameColor;

    glm::vec2 pos = REVBounds.GetPos();
    TextAlignment alignment = TextAlignment::Left;
    switch (REVNameLocation) {
      case REVNameLocationType::None:
      case REVNameLocationType::TopLeft:
        pos = REVBounds.GetPos();
        alignment = TextAlignment::Left;
        break;
      case REVNameLocationType::LeftTop:
        pos = {REVBounds.X - REVNameOffset, Glyphs[0].DestRect.Y};
        alignment = TextAlignment::Right;
        break;
    }

    Vm::Sc3Stream nameStream(NameCode.data());
    Name = TextLayoutPlainLine(nameStream, static_cast<int>(NameCode.size()),
                               DialogueFont, fontSize, ColorTable[colorIndex],
                               1.0f, pos, alignment);
    assert(NameCode.size() == Name.size());
  }
}

void BacklogTextParser::ParseString(BacklogPage& page,
                                    Vm::Sc3VmThread* string) {
  Reset();
  Glyphs.swap(page.Glyphs);
  RubyChunks.swap(page.RubyChunks);
  Name.swap(page.Name);
  PageMode = DPM_REV;
  LastLineStart = Glyphs.size();
  CurrentLineTop = REVBounds.Y;

  ParseString(string);

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

  FontSize = DefaultFontSize;
  BoxBounds = TipsBounds;
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
  PageMode = DPM_TIPS;
  CurrentLineTop = TipsBounds.Y;

  ParseString(string);

  Glyphs.swap(page.Glyphs);
  RubyChunks.swap(page.RubyChunks);
}

}  // namespace Impacto
