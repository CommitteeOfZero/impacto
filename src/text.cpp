#include "text.h"
#include "vm/expression.h"
#include "log.h"
#include "game.h"
#include "renderer2d.h"
#include "animation.h"

#include "profile/charset.h"
#include "profile/dialogue.h"
#include "profile/game.h"

namespace Impacto {

using namespace Impacto::Profile::Dialogue;

DialoguePage* DialoguePages;

static Animation WaitIconAnimation;

enum StringTokenType : uint8_t {
  STT_LineBreak = 0x00,
  STT_CharacterNameStart = 0x01,
  STT_DialogueLineStart = 0x02,
  STT_Present = 0x03,
  STT_SetColor = 0x04,
  STT_Present_Clear = 0x08,
  STT_RubyBaseStart = 0x09,
  STT_RubyTextStart = 0x0A,
  STT_RubyTextEnd = 0x0B,
  STT_SetFontSize = 0x0C,
  STT_PrintInParallel = 0x0E,
  STT_CenterText = 0x0F,
  STT_SetTopMargin = 0x11,
  STT_SetLeftMargin = 0x12,
  STT_GetHardcodedValue = 0x13,
  STT_EvaluateExpression = 0x15,
  STT_Present_0x18 = 0x18,
  STT_AutoForward = 0x19,
  STT_AutoForward_1A = 0x1A,
  STT_RubyCenterPerCharacter = 0x1E,
  STT_AltLineBreak = 0x1F,

  // This is our own!
  STT_Character = 0xFE,

  STT_EndOfString = 0xFF
};

struct StringToken {
  StringTokenType Type;

  uint16_t Val_Uint16;
  int Val_Expr;

  int Read(Vm::Sc3VmThread* ctx);
};

int StringToken::Read(Vm::Sc3VmThread* ctx) {
  int bytesRead = 0;

  uint8_t c = *ctx->Ip;
  ctx->Ip++;
  bytesRead++;
  switch (c) {
    case STT_LineBreak:
    case STT_CharacterNameStart:
    case STT_DialogueLineStart:
    case STT_Present:
    case STT_Present_Clear:
    case STT_RubyBaseStart:
    case STT_RubyTextStart:
    case STT_RubyTextEnd:
    case STT_PrintInParallel:
    case STT_CenterText:
    case STT_Present_0x18:
    case STT_AutoForward:
    case STT_AutoForward_1A:
    case STT_RubyCenterPerCharacter:
    case STT_AltLineBreak:
    case STT_EndOfString: {
      Type = (StringTokenType)c;
      break;
    }

    case STT_SetFontSize:
    case STT_SetTopMargin:
    case STT_SetLeftMargin:
    case STT_GetHardcodedValue: {
      Type = (StringTokenType)c;
      Val_Uint16 = SDL_SwapBE16(*(uint16_t*)(ctx->Ip));
      ctx->Ip += 2;
      bytesRead += 2;
      break;
    }

    case STT_SetColor:
    case STT_EvaluateExpression: {
      Type = (StringTokenType)c;
      uint8_t* oldIp = ctx->Ip;
      // TODO is this really okay to do in parsing code?
      Vm::ExpressionEval(ctx, &Val_Expr);
      bytesRead += (ctx->Ip - oldIp);
      break;
    }

    default: {
      if (c < 0x80) {
        if (c == STT_Character) {
          ImpLog(LL_Error, LC_VM, "STT_Character encountered, uh oh...");
        }
        ImpLog(LL_Error, LC_VM,
               "Encountered unrecognized token %02X in string\n", c);
        Type = STT_EndOfString;
      } else {
        uint16_t glyphId = (((uint16_t)c & 0x7F) << 8) | *ctx->Ip;
        ctx->Ip++;

        Type = STT_Character;
        Val_Uint16 = glyphId;
      }
      break;
    }
  }

  return bytesRead;
}

uint8_t ProcessedTextGlyph::Flags() const {
  return Profile::Charset::Flags[CharId];
}

void TypewriterEffect::Start(int firstGlyph, int glyphCount, float duration) {
  DurationIn = duration;
  FirstGlyph = firstGlyph;
  GlyphCount = glyphCount;
  StartIn(true);
}

float TypewriterEffect::CalcOpacity(int glyph) {
  if (glyph < FirstGlyph) return 1.0f;
  if (glyph >= FirstGlyph + GlyphCount) return 0.0f;

  // We start displaying a glyph after the previous one is 25% opaque
  // => Total time t given glyph count n and time per glyph d:
  //        t = ((n - 1) * (0.25 * d)) + d
  // => Time per glyph given glyph count n and total time t:
  //        d = (4 * t) / (n + 3)

  float currentTime = Progress * DurationIn;
  float timePerGlyph = (4.0f * DurationIn) / ((float)GlyphCount + 3.0f);

  // We animate only [FirstGlyph, FirstGlyph + GlyphCount],
  // shift to [0, GlyphCount]
  int glyphInSeries = glyph - FirstGlyph;
  float glyphStartTime = (float)glyphInSeries * timePerGlyph * 0.25f;
  float glyphEndTime = glyphStartTime + timePerGlyph;

  if (currentTime < glyphStartTime) return 0.0f;
  if (currentTime >= glyphEndTime) return 1.0f;
  return (currentTime - glyphStartTime) / timePerGlyph;
}

bool DialoguePage::TextIsFullyOpaque() { return Typewriter.Progress == 1.0f; }

void DialoguePage::Init() {
  Profile::Dialogue::Configure();
  WaitIconAnimation.DurationIn = Profile::Dialogue::WaitIconAnimationDuration;
  WaitIconAnimation.StartIn();
  WaitIconAnimation.LoopMode = ALM_Loop;

  for (int i = 0; i < Profile::Dialogue::PageCount; i++) {
    DialoguePages[i].Clear();
    DialoguePages[i].Mode = DPM_NVL;
    DialoguePages[i].Id = i;
    DialoguePages[i].FadeAnimation.DurationIn = FadeInDuration;
    DialoguePages[i].FadeAnimation.DurationOut = FadeOutDuration;
  }
}

void DialoguePage::Clear() {
  Length = 0;
  NameLength = 0;
  HasName = false;
  memset(RubyChunks, 0, sizeof(RubyChunk) * DialogueMaxRubyChunks);
  RubyChunkCount = 0;
  CurrentRubyChunk = 0;
  FirstRubyChunkOnLine = 0;
  if (Mode == DPM_ADV) {
    CurrentLineTop = ADVBounds.Y;
  } else {
    CurrentLineTop = NVLBounds.Y;
  }
  CurrentLineTopMargin = 0.0f;
  NVLResetBeforeAdd = false;
}

enum TextParseState { TPS_Normal, TPS_Name, TPS_Ruby };

void DialoguePage::FinishLine(Vm::Sc3VmThread* ctx, int nextLineStart) {
  EndRubyBase(nextLineStart - 1);

  // Lay out all ruby chunks on this line (before we change CurrentLineTop and
  // thus can't find where to put them)
  for (int i = FirstRubyChunkOnLine; i < RubyChunkCount; i++) {
    if (RubyChunks[i].FirstBaseCharacter >= nextLineStart) break;

    uint8_t* oldIp = ctx->Ip;
    ctx->Ip = (uint8_t*)RubyChunks[i].RawText;

    glm::vec2 pos =
        glm::vec2(0, CurrentLineTop + CurrentLineTopMargin + RubyYOffset);

    // ruby base length > ruby text length: block align
    // ruby base length > ruby text length and 0x1E: center per character
    // ruby base length == ruby text length: center per character
    // ruby base length < ruby text length: center over block (handled by block
    // align)

    if (RubyChunks[i].Length == RubyChunks[i].BaseLength ||
        (RubyChunks[i].CenterPerCharacter &&
         RubyChunks[i].BaseLength > RubyChunks[i].Length)) {
      // center every ruby character over the base character below it
      for (int j = 0; j < RubyChunks[i].Length; j++) {
        RectF const& baseGlyphRect =
            Glyphs[RubyChunks[i].FirstBaseCharacter + j].DestRect;
        pos.x = baseGlyphRect.Center().x;
        TextLayoutPlainLine(ctx, 1, RubyChunks[i].Text + j, DialogueFont,
                            RubyFontSize, ColorTable[0], 1.0f, pos,
                            TextAlignment::Center);
      }
    } else {
      // evenly space out all ruby characters over the block of base text
      // TODO is this really the right behaviour for CenterPerCharacter(0x1E)
      // and ruby base length < ruby text length?
      RectF const& baseGlyphRect =
          Glyphs[RubyChunks[i].FirstBaseCharacter].DestRect;
      pos.x = baseGlyphRect.X;
      float blockWidth = 0.0f;
      for (int j = 0; j < RubyChunks[i].BaseLength; j++) {
        blockWidth +=
            Glyphs[RubyChunks[i].FirstBaseCharacter + j].DestRect.Width;
      }
      int rubyLength =
          TextLayoutPlainLine(ctx, RubyChunks[i].Length, RubyChunks[i].Text,
                              DialogueFont, RubyFontSize, ColorTable[0], 1.0f,
                              pos, TextAlignment::Block, blockWidth);
    }

    ctx->Ip = oldIp;

    FirstRubyChunkOnLine++;
  }

  float lineHeight = DefaultFontSize;
  // Glyphs of different font sizes are bottom-aligned within the line
  for (int i = LastLineStart; i < nextLineStart; i++) {
    if (Glyphs[i].DestRect.Height > lineHeight)
      lineHeight = Glyphs[i].DestRect.Height;
  }
  for (int i = LastLineStart; i < nextLineStart; i++) {
    Glyphs[i].DestRect.Y = CurrentLineTop + CurrentLineTopMargin +
                           (lineHeight - Glyphs[i].DestRect.Height);
  }
  CurrentLineTop = CurrentLineTop + CurrentLineTopMargin + lineHeight +
                   DialogueFont->LineSpacing;
  CurrentLineTopMargin = 0.0f;
  LastLineStart = nextLineStart;
}

void DialoguePage::EndRubyBase(int lastBaseCharacter) {
  if (BuildingRubyBase) {
    RubyChunks[CurrentRubyChunk].BaseLength =
        (lastBaseCharacter - RubyChunks[CurrentRubyChunk].FirstBaseCharacter) +
        1;
    BuildingRubyBase = false;
  }
}

void DialoguePage::AddString(Vm::Sc3VmThread* ctx, Audio::AudioStream* voice) {
  if (Mode == DPM_ADV || NVLResetBeforeAdd || PrevMode != Mode) {
    Clear();
  }
  PrevMode = Mode;

  int typewriterStart = Length;

  // TODO should we reset HasName here?
  // It shouldn't really matter since names are an ADV thing and we clear before
  // every add on ADV anyway...

  AutoForward = false;

  float FontSize = DefaultFontSize;
  TextParseState State = TPS_Normal;
  // TODO respect alignment
  TextAlignment Alignment = TextAlignment::Left;
  int LastWordStart = Length;
  LastLineStart = Length;
  DialogueColorPair CurrentColors = ColorTable[0];

  RectF BoxBounds;
  if (Mode == DPM_ADV) {
    BoxBounds = ADVBounds;
  } else {
    BoxBounds = NVLBounds;
  }

  float CurrentX = 0.0f;

  uint16_t name[DialogueMaxNameLength];

  BuildingRubyBase = false;

  StringToken token;
  do {
    token.Read(ctx);
    switch (token.Type) {
      case STT_LineBreak:
      case STT_AltLineBreak: {
        FinishLine(ctx, Length);
        LastWordStart = Length;
        CurrentX = 0.0f;
        break;
      }
      case STT_CharacterNameStart: {
        HasName = true;
        State = TPS_Name;
        break;
      }
      case STT_RubyTextStart: {
        EndRubyBase(Length - 1);
        State = TPS_Ruby;
        break;
      }
      case STT_RubyCenterPerCharacter: {
        RubyChunks[CurrentRubyChunk].CenterPerCharacter = true;
        break;
      }
      case STT_DialogueLineStart: {
        State = TPS_Normal;
        break;
      }
      case STT_RubyTextEnd: {
        // At least S;G uses [ruby-base]link text[ruby-text-end] for mails, with
        // no ruby-text-start
        EndRubyBase(Length - 1);
        State = TPS_Normal;
        break;
      }
      case STT_CenterText: {
        Alignment = TextAlignment::Center;
        break;
      }
      case STT_Present_Clear: {
        NVLResetBeforeAdd = true;
        break;
      }
      case STT_SetLeftMargin: {
        float addX = token.Val_Uint16;
        if (CurrentX + addX > BoxBounds.Width) {
          FinishLine(ctx, Length);
          LastWordStart = Length;
          addX -= (BoxBounds.Width - CurrentX);
          CurrentX = 0.0f;
        }
        while (addX > BoxBounds.Width) {
          FinishLine(ctx, Length);
          addX -= BoxBounds.Width;
        }
        CurrentX += addX;
        break;
      }
      case STT_SetTopMargin: {
        CurrentLineTopMargin = token.Val_Uint16;
        break;
      }
      case STT_SetFontSize: {
        FontSize = ((float)token.Val_Uint16) / 1000.0f;
        break;
      }
      case STT_RubyBaseStart: {
        CurrentRubyChunk = RubyChunkCount;
        RubyChunkCount++;
        RubyChunks[CurrentRubyChunk].FirstBaseCharacter = Length;
        LastWordStart = Length;
        break;
      }
      case STT_AutoForward:
      case STT_AutoForward_1A: {
        AutoForward = true;
        break;
      }
      case STT_SetColor: {
        assert(token.Val_Expr < ColorCount);
        CurrentColors = ColorTable[token.Val_Expr];
      }
      case STT_Character: {
        if (State == TPS_Name) {
          name[NameLength] = SDL_Swap16(token.Val_Uint16 | 0x8000);
          NameLength++;
        } else if (State == TPS_Ruby) {
          RubyChunks[CurrentRubyChunk]
              .RawText[RubyChunks[CurrentRubyChunk].Length] =
              SDL_Swap16(token.Val_Uint16 | 0x8000);
          RubyChunks[CurrentRubyChunk].Length++;
        } else {
          // TODO respect TA_Center
          // TODO what to do about left margin if text alignment is center?

          ProcessedTextGlyph& ptg = Glyphs[Length];
          ptg.CharId = token.Val_Uint16;
          ptg.Opacity = 0.0f;
          ptg.Colors = CurrentColors;

          if (ptg.Flags() & CharacterTypeFlags::WordStartingPunct) {
            LastWordStart = Length;  // still *before* this character
          }

          ptg.DestRect.X = BoxBounds.X + CurrentX;
          ptg.DestRect.Width = (FontSize / DialogueFont->CellHeight) *
                               DialogueFont->Widths[ptg.CharId];
          ptg.DestRect.Height = FontSize;

          CurrentX += ptg.DestRect.Width;

          Length++;

          // Line breaking
          if (ptg.DestRect.X + ptg.DestRect.Width >
              BoxBounds.X + BoxBounds.Width) {
            if (LastLineStart == LastWordStart) {
              // Word doesn't fit on a line, gotta break in the middle of it
              ptg.DestRect.X = BoxBounds.X;
              CurrentX = ptg.DestRect.Width;
              FinishLine(ctx, Length - 1);
              LastWordStart = Length - 1;
            } else {
              int firstNonSpace = LastWordStart;
              // Skip spaces at start of (new) line
              for (int i = LastWordStart; i < Length; i++) {
                if (!(Glyphs[i].Flags() & CharacterTypeFlags::Space)) break;
                firstNonSpace = i + 1;
              }
              FinishLine(ctx, firstNonSpace);
              LastWordStart = firstNonSpace;
              CurrentX = 0.0f;
              for (int i = firstNonSpace; i < Length; i++) {
                Glyphs[i].DestRect.X = BoxBounds.X + CurrentX;
                CurrentX += Glyphs[i].DestRect.Width;
              }
            }
          }

          if (ptg.Flags() & CharacterTypeFlags::WordEndingPunct) {
            LastWordStart = Length;  // now after this character
          }
        }
      }

      // TODO print in parallel
      default: { break; }
    }
  } while (token.Type != STT_EndOfString);

  FinishLine(ctx, Length);
  CurrentX = 0.0f;

  if (HasName) {
    uint8_t* oldIp = ctx->Ip;
    ctx->Ip = (uint8_t*)name;
    int nameLength = TextLayoutPlainLine(ctx, NameLength, Name, DialogueFont,
                                         ADVNameFontSize, ColorTable[0], 1.0f,
                                         ADVNamePos, ADVNameAlignment);
    assert(nameLength == NameLength);
    ctx->Ip = oldIp;
  }

  if (voice != 0) {
    Audio::Channels[Audio::AC_VOICE0].Play(voice, false, 0.0f);
  }

  int typewriterCt = Length - typewriterStart;
  float typewriterDur = 0.0f;
  if (voice != 0) {
    typewriterDur = Audio::Channels[Audio::AC_VOICE0].DurationInSeconds();
  }
  if (typewriterDur <= 0.0f) {
    typewriterDur = (float)typewriterCt / 16.0f;
  }
  Typewriter.Start(typewriterStart, typewriterCt, typewriterDur);
}

void DialoguePage::Update(float dt) {
  Typewriter.Update(dt);

  for (int i = 0; i < Length; i++) {
    Glyphs[i].Opacity = Typewriter.CalcOpacity(i);
  }

  FadeAnimation.Update(dt);
  WaitIconAnimation.Update(dt);
}

void DialoguePage::Render() {
  // dialogue text

  if (FadeAnimation.IsOut()) return;

  glm::vec4 opacityTint(1.0f);
  opacityTint.a = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);

  // Textbox
  if (Mode == DPM_ADV) {
    Renderer2D::DrawSprite(ADVBoxSprite, ADVBoxPos, opacityTint);
  } else {
    glm::vec4 nvlBoxTint(0.0f, 0.0f, 0.0f, opacityTint.a * NVLBoxMaxOpacity);
    Renderer2D::DrawRect(
        RectF(0, 0, Profile::DesignWidth, Profile::DesignHeight), nvlBoxTint);
  }

  Renderer2D::DrawProcessedText(Glyphs, Length, DialogueFont, opacityTint.a,
                                true);

  if (Mode == DPM_ADV && HasName) {
    if (HaveADVNameTag) {
      Renderer2D::DrawSprite(ADVNameTag::LeftSprite, ADVNameTag::Position,
                             opacityTint);
    }

    float width = 0.0f;
    for (int i = 0; i < NameLength; i++) {
      width += Name[i].DestRect.Width;
    }

    if (HaveADVNameTag) {
      // Name graphic additional length
      float lineWidth = width - ADVNameTag::BaseLineWidth;
      float lineX =
          ADVNameTag::Position.x + ADVNameTag::LeftSprite.ScaledWidth();
      while (lineWidth > 0.0f) {
        Sprite lineSprite = ADVNameTag::LineSprite;
        lineSprite.SetScaledWidth(fminf(lineSprite.ScaledWidth(), lineWidth));
        Renderer2D::DrawSprite(
            lineSprite, glm::vec2(lineX, ADVNameTag::Position.y), opacityTint);
        lineX += lineSprite.ScaledWidth();
        lineWidth -= lineSprite.ScaledWidth();
      }
      Renderer2D::DrawSprite(ADVNameTag::RightSprite,
                             glm::vec2(lineX, ADVNameTag::Position.y),
                             opacityTint);
    }

    Renderer2D::DrawProcessedText(Name, NameLength, DialogueFont, opacityTint.a,
                                  true);
  }

  // Wait icon
  if (Typewriter.Progress == 1.0f) {
    Renderer2D::DrawSprite(
        WaitIconSprite,
        glm::vec2(Glyphs[Length - 1].DestRect.X +
                      Glyphs[Length - 1].DestRect.Width + WaitIconOffset.x,
                  Glyphs[Length - 1].DestRect.Y + WaitIconOffset.y),
        opacityTint, glm::vec2(1.0f), WaitIconAnimation.Progress * 2.0f * M_PI);
  }
}

int TextGetStringLength(Vm::Sc3VmThread* ctx) {
  int result = 0;
  StringToken token;
  do {
    result += token.Read(ctx);
  } while (token.Type != STT_EndOfString);
  return result;
}
int TextGetMainCharacterCount(Vm::Sc3VmThread* ctx) {
  int result = 0;
  StringToken token;
  bool isMain = true;
  do {
    switch (token.Type) {
      case STT_CharacterNameStart:
      case STT_RubyTextStart: {
        isMain = false;
        break;
      }
      case STT_DialogueLineStart: {
        isMain = true;
        break;
      }
      case STT_Character: {
        if (isMain) result++;
        break;
      }
    }
  } while (token.Type != STT_EndOfString);
  return result;
}

int TextLayoutPlainLine(Vm::Sc3VmThread* ctx, int stringLength,
                        ProcessedTextGlyph* outGlyphs, Font* font,
                        float fontSize, DialogueColorPair colors, float opacity,
                        glm::vec2 pos, TextAlignment alignment,
                        float blockWidth) {
  int characterCount = 0;
  StringToken token;

  float currentX = 0;

  for (int i = 0; i < stringLength; i++) {
    token.Read(ctx);
    if (token.Type == STT_EndOfString) break;
    if (token.Type != STT_Character) continue;

    ProcessedTextGlyph& ptg = outGlyphs[characterCount];
    ptg.CharId = token.Val_Uint16;
    ptg.Colors = colors;
    ptg.Opacity = opacity;

    ptg.DestRect.X = currentX;
    ptg.DestRect.Y = pos.y;
    ptg.DestRect.Width =
        (fontSize / font->CellHeight) * font->Widths[ptg.CharId];
    ptg.DestRect.Height = fontSize;

    currentX += ptg.DestRect.Width;

    characterCount++;
  }

  // currentX is now line width

  // Block alignment:
  //
  //  l  i  n  e
  // block__below
  //
  // If block below is shorter than line, line is just centered over the block

  if (alignment == +TextAlignment::Block && blockWidth < currentX) {
    pos.x += blockWidth / 2.0f;
    alignment = TextAlignment::Center;
  }

  switch (alignment) {
    case TextAlignment::Left: {
      // pos is top left
      for (int i = 0; i < characterCount; i++) {
        outGlyphs[i].DestRect.X += pos.x;
      }
      break;
    }
    case TextAlignment::Right: {
      // pos is top right
      for (int i = 0; i < characterCount; i++) {
        outGlyphs[i].DestRect.X += (pos.x - currentX);
      }
      break;
    }
    case TextAlignment::Center: {
      // pos is top center
      for (int i = 0; i < characterCount; i++) {
        outGlyphs[i].DestRect.X += (pos.x - (currentX / 2.0f));
      }
      break;
    }
    case TextAlignment::Block: {
      float blockSpacing = blockWidth / (float)currentX;
      if (characterCount >= 1) {
        outGlyphs[0].DestRect.X +=
            pos.x + blockSpacing / 2.0f - outGlyphs[0].DestRect.Width / 2.0f;
      }
      for (int i = 1; i < characterCount; i++) {
        outGlyphs[i].DestRect.X +=
            blockSpacing - outGlyphs[i].DestRect.Width / 2.0f;
      }
      break;
    }
  }

  return characterCount;
}

}  // namespace Impacto