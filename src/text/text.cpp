#include "text.h"

#include <memory>

#include <utf8.h>

#include "../log.h"
#include "../animation.h"
#include "../mem.h"

#include "../profile/scriptvars.h"
#include "../profile/charset.h"
#include "../profile/dialogue.h"
#include "../profile/game.h"
#include "../profile/vm.h"

#include "../vm/expression.h"
#include "../vm/interface/input.h"
#include "../vm/sc3stream.h"

namespace Impacto {
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Dialogue;

int StringToken::Read(Vm::Sc3VmThread* ctx) {
  int bytesRead = 0;
  Flags = 0;

  uint8_t c = *ctx->GetIp();
  ctx->IpOffset++;
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
    case STT_AutoForward_SyncVoice:
    case STT_AutoForward:
    case STT_RubyCenterPerCharacter:
    case STT_AltLineBreak:
    case STT_EndOfString: {
      Type = (StringTokenType)c;
      break;
    }

    case STT_SetFontSize:
    case STT_SetTopMargin:
    case STT_SetLeftMargin:
    case STT_GetHardcodedValue:
    case STT_UnlockTip: {
      Type = (StringTokenType)c;
      Val_Uint16 = (*ctx->GetIp() << 8) | *(ctx->GetIp() + 1);
      ctx->IpOffset += 2;
      bytesRead += 2;
      break;
    }

    case STT_SetColor: {
      Type = (StringTokenType)c;
      if (ColorTagIsUint8) {
        Val_Expr = (*(uint8_t*)(ctx->GetIp()));
        ctx->IpOffset += 1;
        bytesRead += 1;
      } else {
        uint32_t oldIp = ctx->IpOffset;
        // TODO is this really okay to do in parsing code?
        Val_Expr = Vm::ExpressionEval(ctx);
        bytesRead += (int)(ctx->IpOffset - oldIp);
      }
      break;
    }

    case STT_EvaluateExpression: {
      Type = (StringTokenType)c;
      uint32_t oldIp = ctx->IpOffset;
      // TODO is this really okay to do in parsing code?
      Val_Expr = Vm::ExpressionEval(ctx);
      bytesRead += (int)(ctx->IpOffset - oldIp);
      break;
    }

    default: {
      if (c < 0x80) {
        if (c == STT_Character) {
          ImpLog(LogLevel::Error, LogChannel::VM,
                 "STT_Character encountered, uh oh...");
        }
        ImpLog(LogLevel::Error, LogChannel::VM,
               "Encountered unrecognized token 0x{:02x} in string\n", c);
        Type = STT_EndOfString;
      } else {
        uint16_t glyphId = (((uint16_t)c & 0x7F) << 8) | *ctx->GetIp();
        ctx->IpOffset++;

        Flags |= GetFlags(glyphId);

        Type = STT_Character;
        Val_Uint16 = glyphId;
      }
      break;
    }
  }

  return bytesRead;
}

int StringToken::Read(Vm::Sc3Stream& stream) {
  uint8_t c = stream.ReadU8();
  if (c == STT_Character) {
    ImpLog(LogLevel::Error, LogChannel::VM,
           "STT_Character encountered, uh oh...");
    Type = STT_EndOfString;
  } else if (c < 0x80) {
    ImpLog(LogLevel::Error, LogChannel::VM,
           "Encountered non-character token 0x{:02x} in string\n", c);
    Type = STT_EndOfString;
  } else if (c == STT_EndOfString) {
    Type = STT_EndOfString;
  } else {
    uint16_t glyphId = (((uint16_t)c & 0x7F) << 8) | stream.ReadU8();
    Type = STT_Character;
    Val_Uint16 = glyphId;
    return 2;
  }
  return 1;
}

void StringToken::AddFlags(const Vm::BufferOffsetContext scrCtx,
                           const uint8_t flags) {
  Vm::Sc3VmThread dummy;
  dummy.ScriptBufferId = scrCtx.ScriptBufferId;
  dummy.IpOffset = scrCtx.IpOffset;

  StringToken token;
  token.Read(&dummy);
  for (; token.Type != STT_EndOfString; token.Read(&dummy)) {
    if (token.Type != STT_Character) {
      ImpLog(LogLevel::Error, LogChannel::VM,
             "Encountered non-character token 0x{:02x} in flag string\n",
             static_cast<uint8_t>(token.Type));
      return;
    }

    const uint16_t glyphId = token.Val_Uint16;
    if (auto it = FlagsMap.find(glyphId); it != FlagsMap.end()) {
      it->second = it->second | flags;
    } else {
      FlagsMap.emplace(glyphId, flags);
    }
  }
}

int TextGetStringLength(Vm::Sc3Stream& stream) {
  int result = 0;
  StringToken token;
  do {
    result += token.Read(stream);
  } while (token.Type != STT_EndOfString);
  return result;
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
    token.Read(ctx);
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
      default:
        // Probably safe to ignore this
        break;
    }
  } while (token.Type != STT_EndOfString);
  return result;
}

template <typename T>
concept Sc3Type =
    (std::is_lvalue_reference_v<T> &&
     std::is_base_of_v<Vm::Sc3Stream, std::remove_reference_t<T>>) ||
    std::is_same_v<std::decay_t<T>, Vm::Sc3VmThread*>;

std::pair<int, float> TextLayoutPlainLineHelper(
    Sc3Type auto&& sc3, int stringLength,
    std::output_iterator<ProcessedTextGlyph> auto outIt, Font* font,
    float fontSize, DialogueColorPair colors, float opacity, glm::vec2 pos,
    TextAlignment alignment, float blockWidth) {
  size_t characterCount = 0;
  StringToken token;

  float currentX = 0;
  DialogueColorPair currentColors = colors;
  for (int i = 0; i < stringLength; i++) {
    token.Read(sc3);
    if (token.Type == STT_EndOfString) break;

    switch (token.Type) {
      default:
        break;

      case STT_SetColor: {
        if (253 <= token.Val_Expr && token.Val_Expr <= 255) {
          token.Val_Expr = ScrWork[SW_SYSMESCOL1 + (255 - token.Val_Expr)];
        }

        assert(token.Val_Expr < ColorCount);
        currentColors = ColorTable[token.Val_Expr];
      } break;

      case STT_Character: {
        ProcessedTextGlyph ptg;
        ptg.CharId = token.Val_Uint16;
        ptg.Colors = currentColors;
        ptg.Opacity = opacity;

        ptg.DestRect.X = currentX;
        ptg.DestRect.Y = pos.y;
        ptg.DestRect.Width = std::floor((fontSize / font->BitmapEmWidth) *
                                        font->AdvanceWidths[ptg.CharId]);
        ptg.DestRect.Height = fontSize;

        currentX += ptg.DestRect.Width;

        *outIt++ = ptg;
        characterCount++;
      } break;
    }
  }
  // currentX is now line width
  // If you want to align, you can pass a span or vector to the alignment
  // function
  return {static_cast<int>(characterCount), currentX};
}

int TextLayoutPlainLine(Vm::Sc3Stream& stream, int stringLength,
                        std::span<ProcessedTextGlyph> outGlyphs, Font* font,
                        float fontSize, DialogueColorPair colors, float opacity,
                        glm::vec2 pos, TextAlignment alignment,
                        float blockWidth) {
  auto [count, currentX] = TextLayoutPlainLineHelper(
      stream, stringLength, outGlyphs.begin(), font, fontSize, colors, opacity,
      pos, alignment, blockWidth);
  assert(outGlyphs.size() >= static_cast<size_t>(count));
  TextLayoutAlignment(alignment, blockWidth, currentX, pos, count, outGlyphs);
  return count;
}

std::vector<ProcessedTextGlyph> TextLayoutPlainLine(
    Vm::Sc3Stream& stream, int maxLength, Font* font, float fontSize,
    DialogueColorPair colors, float opacity, glm::vec2 pos,
    TextAlignment alignment, float blockWidth) {
  std::vector<ProcessedTextGlyph> outGlyphs;
  outGlyphs.reserve(maxLength);
  auto [count, currentX] = TextLayoutPlainLineHelper(
      stream, maxLength, std::back_inserter(outGlyphs), font, fontSize, colors,
      opacity, pos, alignment, blockWidth);
  TextLayoutAlignment(alignment, blockWidth, currentX, pos, count, outGlyphs);
  return outGlyphs;
}

int TextLayoutPlainLine(Vm::Sc3VmThread* thd, int stringLength,
                        std::span<ProcessedTextGlyph> outGlyphs, Font* font,
                        float fontSize, DialogueColorPair colors, float opacity,
                        glm::vec2 pos, TextAlignment alignment,
                        float blockWidth) {
  auto [count, currentX] = TextLayoutPlainLineHelper(
      thd, stringLength, outGlyphs.begin(), font, fontSize, colors, opacity,
      pos, alignment, blockWidth);
  assert(outGlyphs.size() >= static_cast<size_t>(count));
  TextLayoutAlignment(alignment, blockWidth, currentX, pos, count, outGlyphs);
  return count;
}

std::vector<ProcessedTextGlyph> TextLayoutPlainLine(
    Vm::Sc3VmThread* thd, int maxLength, Font* font, float fontSize,
    DialogueColorPair colors, float opacity, glm::vec2 pos,
    TextAlignment alignment, float blockWidth) {
  std::vector<ProcessedTextGlyph> outGlyphs;
  outGlyphs.reserve(maxLength);
  auto [count, currentX] = TextLayoutPlainLineHelper(
      thd, maxLength, std::back_inserter(outGlyphs), font, fontSize, colors,
      opacity, pos, alignment, blockWidth);
  TextLayoutAlignment(alignment, blockWidth, currentX, pos, count, outGlyphs);
  return outGlyphs;
}

int TextLayoutAlignment(Impacto::TextAlignment& alignment, float blockWidth,
                        float currentX, glm::vec2& pos, int characterCount,
                        std::span<Impacto::ProcessedTextGlyph> outGlyphs) {
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

float TextGetPlainLineWidth(Vm::Sc3VmThread* ctx, Font* font, float fontSize) {
  StringToken token;

  float width = 0.0f;
  while (true) {
    token.Read(ctx);
    if (token.Type == STT_EndOfString) break;
    if (token.Type != STT_Character) continue;

    width += std::floor((fontSize / font->BitmapEmWidth) *
                        font->AdvanceWidths[token.Val_Uint16]);
  }

  return width;
}

float TextGetPlainLineWidth(Vm::Sc3Stream& stream, Font* font, float fontSize) {
  StringToken token;

  float width = 0.0f;
  while (true) {
    token.Read(stream);
    if (token.Type == STT_EndOfString) break;
    if (token.Type != STT_Character) continue;

    width += std::floor((fontSize / font->BitmapEmWidth) *
                        font->AdvanceWidths[token.Val_Uint16]);
  }

  return width;
}

int TextLayoutPlainString(std::string_view str,
                          std::span<ProcessedTextGlyph> outGlyphs, Font* font,
                          float fontSize, DialogueColorPair colors,
                          float opacity, glm::vec2 pos, TextAlignment alignment,
                          float blockWidth) {
  std::string_view::iterator strIt = str.begin();
  std::string_view::iterator strEnd = str.end();

  int sc3StrLength = (int)utf8::distance(strIt, strEnd) + 1;
  std::unique_ptr<uint16_t[]> sc3StrPtr(new uint16_t[sc3StrLength]);

  TextGetSc3String(str,
                   std::span(sc3StrPtr.get(), sc3StrPtr.get() + sc3StrLength));

  Vm::Sc3Stream stream(sc3StrPtr.get());
  return TextLayoutPlainLine(stream, sc3StrLength, outGlyphs, font, fontSize,
                             colors, opacity, pos, alignment, blockWidth);
}

std::vector<ProcessedTextGlyph> TextLayoutPlainString(
    std::string_view str, Font* font, float fontSize, DialogueColorPair colors,
    float opacity, glm::vec2 pos, TextAlignment alignment, float blockWidth) {
  std::string_view::iterator strIt = str.begin();
  std::string_view::iterator strEnd = str.end();

  int sc3StrLength = (int)utf8::distance(strIt, strEnd) + 1;
  std::unique_ptr<uint16_t[]> sc3StrPtr(new uint16_t[sc3StrLength]);

  TextGetSc3String(str,
                   std::span(sc3StrPtr.get(), sc3StrPtr.get() + sc3StrLength));

  Vm::Sc3Stream stream(sc3StrPtr.get());
  return TextLayoutPlainLine(stream, sc3StrLength, font, fontSize, colors,
                             opacity, pos, alignment, blockWidth);
}

void TextGetSc3String(std::string_view str, std::span<uint16_t> out) {
  std::string_view::iterator strIt = str.begin();
  std::string_view::iterator strEnd = str.end();

  [[maybe_unused]] size_t sc3StrLength = (int)utf8::distance(strIt, strEnd) + 1;
  assert(sc3StrLength <= out.size());
  size_t sc3Idx = 0;
  while (strIt != strEnd) {
    auto codePoint = utf8::next(strIt, strEnd);

    uint16_t sc3Val = Profile::Charset::CharacterToSc3[codePoint];
    out[sc3Idx++] = SDL_Swap16(sc3Val);
  }
  out[sc3Idx++] = 0xFF;

  assert(sc3Idx == sc3StrLength);
}

void InitNamePlateData(Vm::Sc3Stream& stream) {
  do {
    uint16_t id = stream.ReadU16();
    uint16_t stringId = stream.ReadU16();
    uint32_t nameAddr =
        Vm::ScriptGetStrAddress(Profile::Vm::SystemScriptBuffer, stringId);
    Vm::Sc3VmThread dummy;
    dummy.IpOffset = nameAddr;
    dummy.ScriptBufferId = Profile::Vm::SystemScriptBuffer;
    int nameLength = (TextGetStringLength(&dummy) - 1) * 2;
    dummy.IpOffset = nameAddr;
    uint32_t nameHash =
        GetHashCode(std::span<uint8_t>(dummy.GetIp(), nameLength));
    NamePlateData[nameHash] = id;
  } while (stream.PeekU16() != 0xFFFF);
}

std::optional<uint32_t> GetNameId(const std::span<const uint16_t> name) {
  uint32_t nameHash = GetHashCode(std::span<const uint8_t>(
      std::bit_cast<uint8_t*>(name.data()), name.size_bytes()));
  if (NamePlateData.find(nameHash) != NamePlateData.end())
    return NamePlateData[nameHash];
  else
    return std::nullopt;
}
}  // namespace Impacto