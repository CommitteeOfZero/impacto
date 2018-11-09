#include "text.h"
#include "vm/expression.h"
#include "log.h"
#include "game.h"

namespace Impacto {

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

  float FontSize = GameCtx->Config.Dlg.DefaultFontSize;
  TextParseState State = TPS_Normal;
  TextAlignment Alignment = TextAlignment::Left;
  int CurrentCharacter = Length;  // in TPS_Normal line
  int LastWordStart = Length;
  int LastLineStart = Length;
  DialogueColorPair CurrentColors = GameCtx->Config.Dlg.ColorTable[0];

  RectF BoxBounds;
  if (Mode == DPM_ADV) {
    BoxBounds = GameCtx->Config.Dlg.ADVBounds;
  } else {
    BoxBounds = GameCtx->Config.Dlg.NVLBounds;
  }

  StringToken token;
  do {
    token.Read(ctx);
    switch (token.Type) {
      case STT_LineBreak:
      case STT_AltLineBreak: {
        CurrentX = 0.0f;
        CurrentY += FontSize;
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
        CurrentColors = GameCtx->Config.Dlg.ColorTable[token.Val_Expr];
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
          ptg.Glyph = GameCtx->Config.Dlg.DialogueFont.Glyph(token.Val_Uint16);
          ptg.CharacterType =
              GameCtx->Config.Dlg.DialogueFont.CharacterType(token.Val_Uint16);
          ptg.Opacity = 0.0f;
          ptg.Colors = CurrentColors;

          if (ptg.CharacterType & CharacterTypeFlags::WordStartingPunct) {
            LastWordStart = Length;  // still *before* this character
          }

          ptg.DestRect.X = BoxBounds.X + CurrentX;
          ptg.DestRect.Y = BoxBounds.Y + CurrentY;
          ptg.DestRect.Width =
              (FontSize / GameCtx->Config.Dlg.DialogueFont.RowHeight()) *
              ptg.Glyph.Bounds.Width;
          ptg.DestRect.Height = FontSize;

          CurrentX += ptg.DestRect.Width;

          Length++;

          // Line breaking
          if (ptg.DestRect.X + ptg.DestRect.Width >
              BoxBounds.X + BoxBounds.Width) {
            CurrentY += FontSize;
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
                Glyphs[i].DestRect.Y += FontSize;
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

  CurrentY += FontSize;  // For NVL no reset
  CurrentX = 0.0f;
}

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
    ADVBoxOpacity -= 1.0f * dt;
    if (ADVBoxOpacity <= 0.0f) {
      ADVBoxOpacity = 0.0f;
      AnimState = DPAS_Hidden;
    }
  } else if (AnimState == DPAS_Showing) {
    ADVBoxOpacity += 1.0f * dt;
    if (ADVBoxOpacity >= 1.0f) {
      ADVBoxOpacity = 1.0f;
      AnimState = DPAS_Shown;
    }
  }
}

void DialoguePage::Render() {
  // dialogue text

  if (AnimState == DPAS_Hidden) return;

  for (int i = 0; i < Length; i++) {
    float opacity = glm::smoothstep(0.0f, 1.0f, Glyphs[i].Opacity);
    if (Mode == DPM_ADV) opacity *= glm::smoothstep(0.0f, 1.0f, ADVBoxOpacity);

    glm::vec4 color = RgbaIntToFloat(Glyphs[i].Colors.TextColor);
    color.a *= opacity;

    GameCtx->R2D->DrawSprite(Glyphs[i].Glyph, Glyphs[i].DestRect, color);
  }

  if (Mode == DPM_ADV && HasName) {
    RectF* dests = (RectF*)ImpStackAlloc(sizeof(RectF) * NameLength);
    Sprite* sprites = (Sprite*)ImpStackAlloc(sizeof(Sprite) * NameLength);

    glm::vec2 pos = GameCtx->Config.Dlg.ADVNamePos;
    float width = 0.0f;
    for (int i = 0; i < NameLength; i++) {
      sprites[i] = GameCtx->Config.Dlg.DialogueFont.Glyph(Name[i]);
      dests[i].X = pos.x;
      dests[i].Y = pos.y;
      dests[i].Width = (GameCtx->Config.Dlg.ADVNameFontSize /
                        GameCtx->Config.Dlg.DialogueFont.RowHeight()) *
                       sprites[i].Bounds.Width;
      dests[i].Height = GameCtx->Config.Dlg.ADVNameFontSize;
      width += dests[i].Width;
      pos.x += dests[i].Width;
    }

    if (GameCtx->Config.Dlg.ADVNameAlignment == +TextAlignment::Center) {
      for (int i = 0; i < NameLength; i++) dests[i].X -= width / 2.0f;
    } else if (GameCtx->Config.Dlg.ADVNameAlignment == +TextAlignment::Right) {
      for (int i = 0; i < NameLength; i++) dests[i].X -= width;
    }

    glm::vec4 color =
        RgbaIntToFloat(GameCtx->Config.Dlg.ColorTable[0].TextColor);
    color.a *= glm::smoothstep(0.0f, 1.0f, ADVBoxOpacity);

    for (int i = 0; i < NameLength; i++) {
      GameCtx->R2D->DrawSprite(sprites[i], dests[i], color);
    }

    ImpStackFree(sprites);
    ImpStackFree(dests);
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