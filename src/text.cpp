#include "text.h"
#include "vm/expression.h"
#include "log.h"
#include "game.h"
#include "renderer2d.h"

#include "profile/charset.h"

namespace Impacto {

DialoguePage DialoguePages[DialoguePageCount];

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
  STT_Unk_1E = 0x1E,
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
    case STT_Unk_1E:
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

void DialoguePage::Clear() {
  Length = 0;
  FullyOpaqueGlyphCount = 0;
  TextIsFullyOpaque = false;
  NameLength = 0;
  HasName = false;
  memset(RubyChunks, 0, sizeof(RubyChunk) * DialogueMaxRubyChunks);
  RubyChunkCount = 0;
  CurrentRubyChunk = 0;
  CurrentX = 0.0f;
  CurrentY = 0.0f;
  NVLResetBeforeAdd = false;
}

enum TextParseState { TPS_Normal, TPS_Name, TPS_Ruby };

void DialoguePage::AddString(Vm::Sc3VmThread* ctx) {
  if (Mode == DPM_ADV || NVLResetBeforeAdd) {
    Clear();
  }

  AutoForward = false;

  float FontSize = Profile::Dlg.DefaultFontSize;
  TextParseState State = TPS_Normal;
  TextAlignment Alignment = TextAlignment::Left;
  int CurrentCharacter = Length;  // in TPS_Normal line
  int LastWordStart = Length;
  int LastLineStart = Length;
  DialogueColorPair CurrentColors = Profile::Dlg.ColorTable[0];

  RectF BoxBounds;
  if (Mode == DPM_ADV) {
    BoxBounds = Profile::Dlg.ADVBounds;
  } else {
    BoxBounds = Profile::Dlg.NVLBounds;
  }

  StringToken token;
  do {
    token.Read(ctx);
    switch (token.Type) {
      case STT_LineBreak:
      case STT_AltLineBreak: {
        CurrentX = 0.0f;
        CurrentY +=
            FontSize + DialoguePageFeatureConfig_RNE.RubyFontSize + 8.0f;
        LastWordStart = Length;
        LastLineStart = Length;
        break;
      }
      case STT_CharacterNameStart: {
        HasName = true;
        State = TPS_Name;
        break;
      }
      case STT_RubyTextStart: {
        CurrentRubyChunk = RubyChunkCount;
        RubyChunkCount++;
        State = TPS_Ruby;
        break;
      }
      case STT_DialogueLineStart: {
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
        CurrentX += token.Val_Uint16;
        break;
      }
      case STT_SetTopMargin: {
        CurrentY += token.Val_Uint16;
        break;
      }
      case STT_SetFontSize: {
        FontSize = token.Val_Uint16;
        break;
      }
      case STT_RubyBaseStart: {
        RubyChunks[CurrentRubyChunk].FirstBaseCharacter = Length;
        break;
      }
      case STT_AutoForward:
      case STT_AutoForward_1A: {
        AutoForward = true;
        break;
      }
      case STT_SetColor: {
        assert(token.Val_Expr < DialogueColors);
        CurrentColors = Profile::Dlg.ColorTable[token.Val_Expr];
      }
      case STT_Character: {
        if (State == TPS_Name) {
          Name[NameLength] = token.Val_Uint16;
          NameLength++;
        } else if (State == TPS_Ruby) {
          RubyChunks[CurrentRubyChunk]
              .Text[RubyChunks[CurrentRubyChunk].Length] = token.Val_Uint16;
          RubyChunks[CurrentRubyChunk].Length++;
        } else {
          // TODO respect TA_Center

          TextIsFullyOpaque = false;
          ProcessedTextGlyph& ptg = Glyphs[Length];
          ptg.Glyph = Profile::Dlg.DialogueFont.Glyph(token.Val_Uint16);
          ptg.CharacterType = Profile::Charset::Flags[token.Val_Uint16];
          ptg.Opacity = 0.0f;
          ptg.Colors = CurrentColors;

          if (ptg.CharacterType & CharacterTypeFlags::WordStartingPunct) {
            LastWordStart = Length;  // still *before* this character
          }

          ptg.DestRect.X = BoxBounds.X + CurrentX;
          ptg.DestRect.Y = BoxBounds.Y + CurrentY;
          ptg.DestRect.Width =
              (FontSize / Profile::Dlg.DialogueFont.RowHeight()) *
              ptg.Glyph.Bounds.Width;
          ptg.DestRect.Height = FontSize;

          CurrentX += ptg.DestRect.Width;

          Length++;

          // Line breaking
          if (ptg.DestRect.X + ptg.DestRect.Width >
              BoxBounds.X + BoxBounds.Width) {
            CurrentY +=
                FontSize + DialoguePageFeatureConfig_RNE.RubyFontSize + 8.0f;
            if (LastLineStart == LastWordStart) {
              // Word doesn't fit on a line, gotta break in the middle of it
              ptg.DestRect.X = BoxBounds.X;
              ptg.DestRect.Y = BoxBounds.Y + CurrentY;
              CurrentX = ptg.DestRect.Width;
              LastLineStart = LastWordStart = Length - 1;
            } else {
              int firstNonSpace = LastWordStart;
              // Skip spaces at start of (new) line
              for (int i = LastWordStart; i < Length; i++) {
                if (!(Glyphs[i].CharacterType & CharacterTypeFlags::Space))
                  break;
                firstNonSpace = i + 1;
              }
              LastLineStart = LastWordStart = firstNonSpace;
              CurrentX = 0.0f;
              for (int i = firstNonSpace; i < Length; i++) {
                Glyphs[i].DestRect.X = BoxBounds.X + CurrentX;
                CurrentX += Glyphs[i].DestRect.Width;
                Glyphs[i].DestRect.Y +=
                    FontSize + DialoguePageFeatureConfig_RNE.RubyFontSize +
                    8.0f;
              }
            }
          }

          if (ptg.CharacterType & CharacterTypeFlags::WordEndingPunct) {
            LastWordStart = Length;  // now after this character
          }
        }
      }

      // TODO print in parallel
      default: { break; }
    }
  } while (token.Type != STT_EndOfString);

  CurrentY += FontSize + DialoguePageFeatureConfig_RNE.RubyFontSize +
              8.0f;  // For NVL no reset
  CurrentX = 0.0f;
}

float WaitIconAngle = 0.0f;

void DialoguePage::Update(float dt) {
  if (!TextIsFullyOpaque) {
    int lastFullyOpaqueGlyphCount = FullyOpaqueGlyphCount;
    for (int i = 0; i < 4; i++) {
      int ch = lastFullyOpaqueGlyphCount + i;
      if (ch == Length) break;
      if (Glyphs[ch].Opacity == 1.0f) {
        FullyOpaqueGlyphCount = ch + 1;
        if (FullyOpaqueGlyphCount == Length) {
          TextIsFullyOpaque = true;
          break;
        }
      }
      // TODO speed adjustment
      Glyphs[ch].Opacity = fminf(Glyphs[ch].Opacity + 4.0f * dt, 1.0f);
      if (Glyphs[ch].Opacity < 0.25f) break;
    }
  }

  if (AnimState == DPAS_Hiding) {
    ADVBoxOpacity -= 3.0f * dt;
    if (ADVBoxOpacity <= 0.0f) {
      ADVBoxOpacity = 0.0f;
      AnimState = DPAS_Hidden;
    }
  } else if (AnimState == DPAS_Showing) {
    ADVBoxOpacity += 3.0f * dt;
    if (ADVBoxOpacity >= 1.0f) {
      ADVBoxOpacity = 1.0f;
      AnimState = DPAS_Shown;
    }
  }

  if (WaitIconAngle >= 6.28319f) WaitIconAngle = 0.0f;
  WaitIconAngle += 1.8f * dt;
}

void DialoguePage::Render() {
  // dialogue text

  if (AnimState == DPAS_Hidden) return;

  // Textbox
  if (Mode == DPM_ADV) {
    Sprite mesBox;
    mesBox.Sheet = Profile::Dlg.DataSpriteSheet;
    mesBox.Bounds = RectF(768.0f, 807.0f, 1280.0f, 206.0f);
    mesBox.BaseScale = glm::vec2(1280.0f / 960.0f, 720.0f / 544.0f);
    glm::vec4 col;
    col.r = 1.0f;
    col.g = 1.0f;
    col.b = 1.0f;
    col.a = glm::smoothstep(0.0f, 1.0f, ADVBoxOpacity);
    Renderer2D::DrawSprite(mesBox, glm::vec2(0.0f, 361.0f * (720.0f / 544.0f)),
                           col);
  }

  for (int i = 0; i < Length; i++) {
    float opacity = glm::smoothstep(0.0f, 1.0f, Glyphs[i].Opacity);
    if (Mode == DPM_ADV) opacity *= glm::smoothstep(0.0f, 1.0f, ADVBoxOpacity);

    glm::vec4 color = RgbaIntToFloat(Glyphs[i].Colors.TextColor);
    color.a *= opacity;

    // Outline, the dirty way
    ///////////////////////////////////////////////////////////////////////////////
    glm::vec4 outcolor = glm::vec4(0.0f, 0.0f, 0.0f, color.a);
    Renderer2D::DrawSprite(
        Glyphs[i].Glyph,
        RectF(Glyphs[i].DestRect.X + 1, Glyphs[i].DestRect.Y + 1,
              Glyphs[i].DestRect.Width, Glyphs[i].DestRect.Height),
        outcolor);
    Renderer2D::DrawSprite(
        Glyphs[i].Glyph,
        RectF(Glyphs[i].DestRect.X - 1, Glyphs[i].DestRect.Y - 1,
              Glyphs[i].DestRect.Width, Glyphs[i].DestRect.Height),
        outcolor);
    ///////////////////////////////////////////////////////////////////////////////

    Renderer2D::DrawSprite(Glyphs[i].Glyph, Glyphs[i].DestRect, color);
  }

  if (Mode == DPM_ADV && HasName) {
    Sprite nameInd;
    nameInd.Sheet = Profile::Dlg.DataSpriteSheet;
    nameInd.Bounds = RectF(768.0f, 774.0f, 155.0f, 31.0f);
    nameInd.BaseScale = glm::vec2(1280.0f / 960.0f, 720.0f / 544.0f);
    glm::vec4 col;
    col.r = 1.0f;
    col.g = 1.0f;
    col.b = 1.0f;
    col.a = glm::smoothstep(0.0f, 1.0f, ADVBoxOpacity);
    Renderer2D::DrawSprite(nameInd, glm::vec2(0.0f, 380.0f * (720.0f / 544.0f)),
                           col);

    RectF* dests = (RectF*)ImpStackAlloc(sizeof(RectF) * NameLength);
    Sprite* sprites = (Sprite*)ImpStackAlloc(sizeof(Sprite) * NameLength);

    glm::vec2 pos = Profile::Dlg.ADVNamePos;
    float width = 0.0f;
    for (int i = 0; i < NameLength; i++) {
      sprites[i] = Profile::Dlg.DialogueFont.Glyph(Name[i]);
      dests[i].X = pos.x;
      dests[i].Y = pos.y;
      dests[i].Width = (Profile::Dlg.ADVNameFontSize /
                        Profile::Dlg.DialogueFont.RowHeight()) *
                       sprites[i].Bounds.Width;
      dests[i].Height = Profile::Dlg.ADVNameFontSize;
      width += dests[i].Width;
      pos.x += dests[i].Width;
    }

    if (Profile::Dlg.ADVNameAlignment == +TextAlignment::Center) {
      for (int i = 0; i < NameLength; i++) dests[i].X -= width / 2.0f;
    } else if (Profile::Dlg.ADVNameAlignment == +TextAlignment::Right) {
      for (int i = 0; i < NameLength; i++) dests[i].X -= width;
    }

    // Name graphic additional length
    float graphicAddWidth = width * 0.75f;
    if (graphicAddWidth > 48.0f)
      graphicAddWidth -= 48.0f;
    else
      graphicAddWidth = 0.0f;
    Sprite nameGraphicAdd;
    nameGraphicAdd.Sheet = Profile::Dlg.DataSpriteSheet;
    nameGraphicAdd.BaseScale = glm::vec2(1280.0f / 960.0f, 720.0f / 544.0f);
    float nameGraphicPos = 155.0f;

    while (graphicAddWidth > 0.0f) {
      if (graphicAddWidth > 86.0f) {
        nameGraphicAdd.Bounds = RectF(923.0f, 774.0f, 86.0f, 31.0f);
        Renderer2D::DrawSprite(nameGraphicAdd,
                               RectF(nameGraphicPos * (1280.0f / 960.0f),
                                     380.0f * (720.0f / 544.0f), 86.0f, 31.0f),
                               col);
        graphicAddWidth -= 86.0f;
        nameGraphicPos += 86.0f;
      } else {
        nameGraphicAdd.Bounds = RectF(923.0f, 774.0f, graphicAddWidth, 31.0f);
        Renderer2D::DrawSprite(
            nameGraphicAdd,
            RectF(nameGraphicPos * (1280.0f / 960.0f),
                  380.0f * (720.0f / 544.0f), graphicAddWidth, 31.0f),
            col);
        nameGraphicPos += graphicAddWidth;
        graphicAddWidth = 0.0f;
      }
    }

    Sprite nameGraphicDot;
    nameGraphicDot.Sheet = Profile::Dlg.DataSpriteSheet;
    nameGraphicDot.BaseScale = glm::vec2(1280.0f / 960.0f, 720.0f / 544.0f);
    nameGraphicDot.Bounds = RectF(1010.0f, 774.0f, 22.0f, 31.0f);
    Renderer2D::DrawSprite(nameGraphicDot,
                           RectF(nameGraphicPos * (1280.0f / 960.0f),
                                 380.0f * (720.0f / 544.0f), 22.0f, 31.0f),
                           col);

    glm::vec4 color = RgbaIntToFloat(Profile::Dlg.ColorTable[0].TextColor);
    color.a *= glm::smoothstep(0.0f, 1.0f, ADVBoxOpacity);

    glm::vec4 outcolor = glm::vec4(0.0f, 0.0f, 0.0f, color.a);

    for (int i = 0; i < NameLength; i++) {
      // Name outline, the dirty way
      ///////////////////////////////////////////////////////////////////////////////
      Renderer2D::DrawSprite(sprites[i],
                             RectF(dests[i].X + 1, dests[i].Y + 1,
                                   dests[i].Width, dests[i].Height),
                             outcolor);
      Renderer2D::DrawSprite(sprites[i],
                             RectF(dests[i].X - 1, dests[i].Y - 1,
                                   dests[i].Width, dests[i].Height),
                             outcolor);
      ///////////////////////////////////////////////////////////////////////////////
      Renderer2D::DrawSprite(sprites[i], dests[i], color);
    }

    ImpStackFree(sprites);
    ImpStackFree(dests);
  }

  // Wait icon
  if (TextIsFullyOpaque) {
    Sprite waitIcon;
    waitIcon.Sheet = Profile::Dlg.DataSpriteSheet;
    waitIcon.Bounds = RectF(1.0f, 97.0f, 32.0f, 32.0f);
    waitIcon.BaseScale = glm::vec2(1.0f);
    Renderer2D::DrawSprite(
        waitIcon,
        RectF(Glyphs[Length - 1].DestRect.X +
                  Glyphs[Length - 1].DestRect.Width + 4.0f,
              Glyphs[Length - 1].DestRect.Y + 4.0f, 32.0f, 32.0f),
        glm::vec4(1.0), WaitIconAngle);
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

}  // namespace Impacto