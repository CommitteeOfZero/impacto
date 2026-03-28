#include "textparser.h"

#include "../profile/dialogue.h"

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
  LastRubyBaseCharacter = 0;
  FirstRubyChunkOnLine = 0;

  ParallelStartGlyphs.clear();

  PageMode = DPM_ADV;
  BoxBounds = ADVBounds;

  LastLineStart = 0;
  LastWordStart = 0;
  PrevGlyphWordStarting = false;
  PrevGlyphWordEnding = false;

  FontSize = DefaultFontSize;

  Alignment = TextAlignment::Left;
  CurrentX = 0.0f;
  CurrentLineTop = 0.0f;
  CurrentLineTopMargin = 0.0f;

  CurrentColors = ColorTable[0];
}

void TextParser::ParseLineBreak(const StringToken& token) {
  FinishLine(Glyphs.size());
  LastWordStart = Glyphs.size();
  CurrentX = 0.0f;
}

void TextParser::ParseCharacterNameStart(const StringToken& token) {
  NameCode.reserve(64);
  ParsingState = TextParsingState::Name;
  if (PageMode == DPM_REV && REVNameLocation != REVNameLocationType::LeftTop) {
    CurrentLineTop += REVNameOffset;
  }
  LastWordStart = Glyphs.size();
}

void TextParser::ParseDialogueLineStart(const StringToken& token) {
  ParsingState = TextParsingState::Normal;
  LastWordStart = Glyphs.size();
}

void TextParser::ParsePresent(const StringToken& token) {
  AdvanceMethod = DialoguePage::AdvanceMethodType::Present;
}

void TextParser::ParseSetColor(const StringToken& token) {
  if (PageMode == DPM_REV) return;

  assert(token.Val_Expr < ColorCount);
  CurrentColors = ColorTable[token.Val_Expr];
}

void TextParser::ParsePresentClear(const StringToken& token) {
  AdvanceMethod = DialoguePage::AdvanceMethodType::PresentClear;
}

void TextParser::ParseRubyBaseStart(const StringToken& token) {
  RubyChunks.emplace_back().FirstBaseCharacter = Glyphs.size();
  BuildingRubyBase = true;
  LastWordStart = Glyphs.size();
}

void TextParser::ParseRubyTextStart(const StringToken& token) {
  EndRubyBase();
  ParsingState = TextParsingState::Ruby;
}

void TextParser::ParseRubyTextEnd(const StringToken& token) {
  // At least S;G uses [ruby-base]link text[ruby-text-end] for mails,
  // with no ruby-text-start
  EndRubyBase();
  ParsingState = TextParsingState::Normal;
  LastWordStart = Glyphs.size();
}

void TextParser::ParseSetFontSize(const StringToken& token) {
  FontSize = DefaultFontSize * (token.Val_Uint16 / SetFontSizeRatio);
}

void TextParser::ParsePrintInParallel(const StringToken& token) {
  ParallelStartGlyphs.insert(Glyphs.size());
}

void TextParser::ParseCenterText(const StringToken& token) {
  Alignment = TextAlignment::Center;
}

void TextParser::ParseSetTopMargin(const StringToken& token) {
  CurrentLineTopMargin = token.Val_Uint16;
}

void TextParser::ParseSetLeftMargin(const StringToken& token) {
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
  LastWordStart = Glyphs.size();
}

void TextParser::ParseUnlockTip(const StringToken& token) {
  if ((PageMode == DPM_ADV ||
       (ScrWork[SW_MESWIN0TYPE] == 1 && PageMode == DPM_REV) ||
       PageMode == DPM_NVL) &&
      TipsSystem::GetTipLockedState(token.Val_Uint16)) {
    TipsSystem::SetTipLockedState(token.Val_Uint16, false);
    TipsNotification::AddTip(token.Val_Uint16);
    TipsSystem::GetNewTipsIndices().push_back(token.Val_Uint16);
  }
}

void TextParser::ParsePreset0x18(const StringToken& token) {
  AdvanceMethod = DialoguePage::AdvanceMethodType::Present0x18;
}

void TextParser::ParseAutoForwardSyncVoice(const StringToken& token) {
  AdvanceMethod = DialoguePage::AdvanceMethodType::AutoForwardSyncVoice;
}

void TextParser::ParseAutoForward(const StringToken& token) {
  AdvanceMethod = DialoguePage::AdvanceMethodType::AutoForward;
}

void TextParser::ParseRubyCenterPerCharacter(const StringToken& token) {
  RubyChunks.back().CenterPerCharacter = true;
}

void TextParser::ParseCharacter(const StringToken& token) {
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

      if (token.Flags & +CharacterTypeFlags::WordStartingPunct) {
        // Ensure only the leftmost consecutive WordStartingPunct is counted
        if (!PrevGlyphWordStarting) {
          // Still *before* this character
          LastWordStart = Glyphs.size() - 1;
          PrevGlyphWordStarting = true;
        }
      } else {
        PrevGlyphWordStarting = false;
      }

      if (token.Flags & +CharacterTypeFlags::WordEndingPunct) {
        PrevGlyphWordEnding = true;
        // Ensure only the rightmost consecutive WordEndingPunct is counted
      } else if (PrevGlyphWordEnding) {
        // Previous character was word ending, so this character marks the
        // beginning of the next word
        LastWordStart = Glyphs.size() - 1;
        PrevGlyphWordEnding = false;
      }

      glyph.DestRect.X = BoxBounds.X + CurrentX;
      glyph.DestRect.Width = (FontSize / DialogueFont->BitmapEmWidth) *
                             DialogueFont->AdvanceWidths[glyph.CharId];
      glyph.DestRect.Height = FontSize;

      CurrentX += glyph.DestRect.Width;

      // Line breaking
      if (glyph.DestRect.Right() > BoxBounds.Right()) {
        if (LastLineStart == LastWordStart) {
          // Word doesn't fit on a line, gotta break in the middle of it
          glyph.DestRect.X = BoxBounds.X;
          CurrentX = glyph.DestRect.Width;

          LastWordStart = Glyphs.size() - 1;
          FinishLine(LastWordStart);
        } else {
          size_t firstNonSpace = LastWordStart;
          // Skip spaces at start of (new) line
          for (size_t i = LastWordStart; i < Glyphs.size(); i++) {
            const bool isSpace = StringToken::GetFlags(Glyphs[i].CharId) &
                                 +CharacterTypeFlags::Space;
            if (!isSpace) break;
            firstNonSpace = i + 1;
          }

          LastWordStart = firstNonSpace;
          FinishLine(firstNonSpace);

          CurrentX = 0.0f;
          for (size_t i = firstNonSpace; i < Glyphs.size(); i++) {
            Glyphs[i].DestRect.X = BoxBounds.X + CurrentX;
            CurrentX += Glyphs[i].DestRect.Width;
          }
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
    const size_t firstBaseCharacter = RubyChunks.back().FirstBaseCharacter;
    RubyChunks.back().BaseLength = LastRubyBaseCharacter - firstBaseCharacter;
  }

  BuildingRubyBase = false;
}

void DialogueTextParser::ParseString(Vm::Sc3VmThread* string) {
  const ankerl::unordered_dense::map<
      StringTokenType,
      std::function<void(DialogueTextParser*, const StringToken&)>>
      parsingFunctions{
          {STT_LineBreak, &DialogueTextParser::ParseLineBreak},
          {STT_CharacterNameStart,
           &DialogueTextParser::ParseCharacterNameStart},
          {STT_DialogueLineStart, &DialogueTextParser::ParseDialogueLineStart},
          {STT_Present, &DialogueTextParser::ParsePresent},
          {STT_SetColor, &DialogueTextParser::ParseSetColor},
          {STT_Present_Clear, &DialogueTextParser::ParsePresentClear},
          {STT_RubyBaseStart, &DialogueTextParser::ParseRubyBaseStart},
          {STT_RubyTextStart, &DialogueTextParser::ParseRubyTextStart},
          {STT_RubyTextEnd, &DialogueTextParser::ParseRubyTextEnd},
          {STT_SetFontSize, &DialogueTextParser::ParseSetFontSize},
          {STT_PrintInParallel, &DialogueTextParser::ParsePrintInParallel},
          {STT_CenterText, &DialogueTextParser::ParseCenterText},
          {STT_SetTopMargin, &DialogueTextParser::ParseSetTopMargin},
          {STT_SetLeftMargin, &DialogueTextParser::ParseSetLeftMargin},
          {STT_UnlockTip, &DialogueTextParser::ParseUnlockTip},
          {STT_Present_0x18, &DialogueTextParser::ParsePresent},
          {STT_AutoForward_SyncVoice,
           &DialogueTextParser::ParseAutoForwardSyncVoice},
          {STT_AutoForward, &DialogueTextParser::ParseAutoForward},
          {STT_RubyCenterPerCharacter,
           &DialogueTextParser::ParseRubyCenterPerCharacter},
          {STT_AltLineBreak, &DialogueTextParser::ParseLineBreak},
          {STT_Character, &DialogueTextParser::ParseCharacter},
      };

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
    const auto function = parsingFunctions.find(token.Type);
    if (function != parsingFunctions.end()) function->second(this, token);
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

}  // namespace Impacto
