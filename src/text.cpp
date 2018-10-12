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
    case STT_EndOfString:
      Type = (StringTokenType)c;
      break;

    case STT_SetFontSize:
    case STT_SetTopMargin:
    case STT_SetLeftMargin:
    case STT_GetHardcodedValue:
      Type = (StringTokenType)c;
      Val_Uint16 = SDL_SwapBE16(*(uint16_t*)(ctx->Ip));
      ctx->Ip += 2;
      bytesRead += 2;
      break;

    case STT_SetColor:
    case STT_EvaluateExpression:
      Type = (StringTokenType)c;
      uint8_t* oldIp = ctx->Ip;
      // TODO is this really okay to do in parsing code?
      Vm::ExpressionEval(ctx, &Val_Expr);
      bytesRead += (ctx->Ip - oldIp);
      break;

    default:
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

  return bytesRead;
}

void DialoguePage::Clear() {
  Length = 0;
  FullyOpaqueGlyphCount = 0;
  IsFullyOpaque = false;
  NameLength = 0;
  HasName = false;
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
  TextAlignment Alignment = TA_Left;
  int CurrentCharacter = Length;  // in TPS_Normal line
  int LastWordStart = Length;
  glm::vec4 CurrentColor = GetCurrentBaseColor();

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
      case STT_AltLineBreak:
        CurrentX = 0.0f;
        CurrentY += FontSize;
        LastWordStart = Length;
        break;
      case STT_CharacterNameStart:
        HasName = true;
        State = TPS_Name;
        break;
      case STT_RubyTextStart:
        CurrentRubyChunk = RubyChunkCount;
        RubyChunkCount++;
        State = TPS_Ruby;
        break;
      case STT_DialogueLineStart:
        State = TPS_Normal;
        break;
      case STT_CenterText:
        Alignment = TA_Center;
        break;
      case STT_Present_Clear:
        NVLResetBeforeAdd = true;
        break;
      case STT_SetLeftMargin:
        CurrentX += token.Val_Uint16;
        break;
      case STT_SetTopMargin:
        CurrentY += token.Val_Uint16;
        break;
      case STT_SetFontSize:
        FontSize = token.Val_Uint16;
        break;
      case STT_RubyBaseStart:
        RubyChunks[CurrentRubyChunk].FirstBaseCharacter = Length;
        break;
      case STT_AutoForward:
      case STT_AutoForward_1A:
        AutoForward = true;
        break;

      case STT_Character: {
        if (State == TPS_Name) {
          Name[NameLength] = token.Val_Uint16;
          NameLength++;
        } else if (State == TPS_Ruby) {
          RubyChunks[CurrentRubyChunk]
              .Text[RubyChunks[CurrentRubyChunk].Length] = token.Val_Uint16;
          RubyChunks[CurrentRubyChunk].Length++;
        } else {
          // TODO get rid of this hack and also do more sophisticated wordwrap
          if (token.Val_Uint16 == 0 || token.Val_Uint16 == 63) {  // space
            LastWordStart = Length;
          }

          IsFullyOpaque = false;
          ProcessedTextGlyph& ptg = Glyphs[Length];
          ptg.Glyph = FontHandle->Glyph(token.Val_Uint16);
          ptg.Opacity = 0.0f;
          ptg.Color = CurrentColor;

          ptg.DestRect.X = BoxBounds.X + CurrentX;
          ptg.DestRect.Y = BoxBounds.Y + CurrentY;
          ptg.DestRect.Width =
              (FontSize / FontHandle->RowHeight) * ptg.Glyph.Bounds.Width;
          ptg.DestRect.Height = FontSize;

          Length++;

          // Wordwrap
          if (ptg.DestRect.X + ptg.DestRect.Width >
              BoxBounds.X + BoxBounds.Width) {
            for (int i = 0; i < Length; i++) {
              Glyphs[i].DestRect.X -= CurrentX;
              Glyphs[i].DestRect.Y += FontSize;
            }
            CurrentX = 0.0f;
            CurrentY += FontSize;
            LastWordStart = Length;
          }
        }
      }

      // TODO print in parallel, set color
      default:
        break;
    }
  } while (token.Type != STT_EndOfString);
}

glm::vec4 GetCurrentBaseColor() {
  // TODO get this from ScrWork?
  return glm::vec4(1.0f);
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