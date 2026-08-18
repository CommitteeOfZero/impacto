#include "text.h"

#include <memory>

#include <utf8cpp/utf8.h>

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

    AddFlags(token.Val_Uint16, flags);
  }
}

[[nodiscard]] static size_t TextGetStringLength(Sc3Type auto&& stream) {
  size_t result = 0;
  StringToken token;
  do {
    result += token.Read(stream);
  } while (token.Type != STT_EndOfString);
  return result;
}

template size_t TextLayoutPlainLine(Vm::Sc3VmThread*&&,
                                    std::span<ProcessedTextGlyph>, const Font&,
                                    float, DialogueColorPair, float, glm::vec2,
                                    TextAlignment, float);
size_t TextLayoutPlainLine(Sc3Type auto&& stream,
                           const std::span<ProcessedTextGlyph> outGlyphs,
                           const Font& font, const float fontSize,
                           const DialogueColorPair colors, const float opacity,
                           const glm::vec2 pos, const TextAlignment alignment,
                           const float blockWidth) {
  size_t characterCount = 0;
  StringToken token;

  float currentX = 0.0f;
  DialogueColorPair currentColors = colors;
  for (auto curGlyph = outGlyphs.begin(); curGlyph != outGlyphs.end();) {
    token.Read(stream);
    if (token.Type == STT_EndOfString) break;

    switch (token.Type) {
      default:
        break;

      case STT_SetColor: {
        if (253 <= token.Val_Expr && token.Val_Expr <= 255) {
          token.Val_Expr = ScrWork[SW_SYSMESCOL1 + (255 - token.Val_Expr)];
        }

        currentColors = ColorTable[token.Val_Expr];
      } break;

      case STT_Character: {
        const uint32_t glyphId = token.Val_Uint16;

        *curGlyph = font.PlaceGlyph(glyphId, {currentX, pos.y}, fontSize,
                                    currentColors, opacity);
        curGlyph++;
        currentX +=
            font.GetAdvanceWidth(glyphId) * fontSize / font.BitmapEmWidth;
        characterCount++;
      } break;
    }
  }

  if (characterCount > 0) {
    TextLayoutAlignment(alignment, pos.x, outGlyphs.subspan(0, characterCount),
                        blockWidth);
  }

  return characterCount;
}

template std::vector<ProcessedTextGlyph> TextLayoutPlainLine(
    Vm::Sc3Stream&, std::optional<size_t>, const Font&, float,
    DialogueColorPair, float, glm::vec2, TextAlignment, float);
template std::vector<ProcessedTextGlyph> TextLayoutPlainLine(
    Vm::Sc3VmThread*&&, std::optional<size_t>, const Font&, float,
    DialogueColorPair, float, glm::vec2, TextAlignment, float);
std::vector<ProcessedTextGlyph> TextLayoutPlainLine(
    Sc3Type auto&& stream, std::optional<size_t> maxLength, const Font& font,
    const float fontSize, const DialogueColorPair colors, const float opacity,
    const glm::vec2 pos, const TextAlignment alignment,
    const float blockWidth) {
  if (!maxLength.has_value()) {
    auto lengthStream = stream;
    maxLength.emplace(TextGetStringLength(lengthStream));
  }

  std::vector<ProcessedTextGlyph> outGlyphs(*maxLength);
  const size_t glyphCount =
      TextLayoutPlainLine(stream, outGlyphs, font, fontSize, colors, opacity,
                          pos, alignment, blockWidth);
  outGlyphs.resize(glyphCount);

  if (!outGlyphs.empty()) {
    TextLayoutAlignment(alignment, pos.x, outGlyphs, blockWidth);
  }

  return outGlyphs;
}

void TextLayoutAlignment(TextAlignment alignment, float posX,
                         const std::span<ProcessedTextGlyph> outGlyphs,
                         const float blockWidth) {
  assert(!(alignment == TextAlignment::Block && blockWidth == 0.0f) &&
         "Forgot to specify blockWidth for Block alignment");
  if (outGlyphs.empty()) return;

  const float left = outGlyphs.front().DestRect.Left();
  const float right = outGlyphs.back().DestRect.Right();
  const float textWidth = right - left;

  // Block alignment:
  //
  //  l  i  n  e
  // block__below
  //
  // If block below is shorter than line, line is just centered over the block
  if (alignment == TextAlignment::Block && blockWidth < textWidth) {
    posX += blockWidth / 2.0f;
    alignment = TextAlignment::Center;
  }

  switch (alignment) {
    case TextAlignment::Left: {
      // posX is left
      const float offset = posX - left;
      for (ProcessedTextGlyph& glyph : outGlyphs) {
        glyph.DestRect.X += offset;
      }
    } break;

    case TextAlignment::Right: {
      // posX is right
      const float offset = posX - right;
      for (ProcessedTextGlyph& glyph : outGlyphs) {
        glyph.DestRect.X += offset;
      }
    } break;

    case TextAlignment::Center: {
      // posX is center
      const float offset = posX - textWidth / 2.0f;
      for (ProcessedTextGlyph& glyph : outGlyphs) {
        glyph.DestRect.X += offset;
      }
    } break;

    case TextAlignment::Block: {
      // posX is left
      const float offset = posX - left;
      const float glyphSpacing =
          (blockWidth - textWidth) / (outGlyphs.size() - 1);

      for (size_t idx = 0; idx < outGlyphs.size(); idx++) {
        outGlyphs[idx].DestRect.X += offset + glyphSpacing * idx;
      }
    } break;
  }
}

template float TextGetPlainLineWidth(Vm::Sc3Stream&, const Font&, float);
template float TextGetPlainLineWidth(Vm::Sc3VmThread*&&, const Font&, float);
float TextGetPlainLineWidth(Sc3Type auto&& stream, const Font& font,
                            const float fontSize) {
  StringToken token;

  float width = 0.0f;
  while (true) {
    token.Read(stream);
    if (token.Type == STT_EndOfString) break;
    if (token.Type != STT_Character) continue;

    width += (fontSize / font.BitmapEmWidth) *
             font.GetAdvanceWidth(token.Val_Uint16);
  }

  return width;
}

size_t TextLayoutPlainString(const std::string_view str,
                             const std::span<ProcessedTextGlyph> outGlyphs,
                             const Font& font, const float fontSize,
                             const DialogueColorPair colors,
                             const float opacity, const glm::vec2 pos,
                             const TextAlignment alignment,
                             const float blockWidth) {
  size_t sc3StrLength = utf8::distance(str.begin(), str.end()) + 1;
  assert(outGlyphs.size() == sc3StrLength - 1);
  std::vector<uint16_t> sc3Str(sc3StrLength);

  TextGetSc3String(str, sc3Str);

  Vm::Sc3Stream stream(sc3Str.data());
  return TextLayoutPlainLine(stream, outGlyphs, font, fontSize, colors, opacity,
                             pos, alignment, blockWidth);
}

std::vector<ProcessedTextGlyph> TextLayoutPlainString(
    const std::string_view str, const Font& font, const float fontSize,
    const DialogueColorPair colors, const float opacity, const glm::vec2 pos,
    const TextAlignment alignment, const float blockWidth) {
  const size_t stringLength = utf8::distance(str.begin(), str.end());
  std::vector<ProcessedTextGlyph> outGlyphs(stringLength);

  TextLayoutPlainString(str, outGlyphs, font, fontSize, colors, opacity, pos,
                        alignment, blockWidth);

  return outGlyphs;
}

void TextGetSc3String(const std::string_view str,
                      const std::span<uint16_t> out) {
  std::string_view::iterator strIt = str.begin();
  std::string_view::iterator strEnd = str.end();

  assert(static_cast<size_t>(utf8::distance(strIt, strEnd)) + 1 <= out.size());

  size_t sc3Idx = 0;
  while (strIt != strEnd) {
    const auto codePoint = utf8::next(strIt, strEnd);

    const uint16_t sc3Val = Profile::Charset::CharacterToSc3[codePoint];
    out[sc3Idx++] = SDL_Swap16(sc3Val);
  }
  out[sc3Idx++] = 0xFF;
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
    size_t nameLength = (TextGetStringLength(&dummy) - 1) * 2;
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