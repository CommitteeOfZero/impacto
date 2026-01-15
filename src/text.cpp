#include <memory>
#include "text.h"
#include "vm/expression.h"
#include "log.h"
#include "animation.h"
#include "mem.h"
#include "profile/scriptvars.h"
#include "profile/configsystem.h"

#include "profile/charset.h"
#include "profile/dialogue.h"
#include "profile/game.h"
#include "profile/vm.h"

#include "data/tipssystem.h"

#include "hud/waiticondisplay.h"
#include "hud/autoicondisplay.h"
#include "hud/skipicondisplay.h"
#include "hud/nametagdisplay.h"
#include "hud/dialoguebox.h"
#include "hud/tipsnotification.h"
#include "games/mo6tw/dialoguebox.h"
#include "games/chlcc/dialoguebox.h"
#include "profile/games/chlcc/dialoguebox.h"
#include "games/cc/dialoguebox.h"
#include "audio/audiostream.h"

#include <utf8.h>
#include "vm/interface/input.h"
#include "vm/sc3stream.h"

namespace Impacto {
using namespace Impacto::Profile::Dialogue;
using namespace Impacto::Profile::ScriptVars;

static DialogueBox* TextBox;

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

void TypewriterEffect::Start(const bool voiced) {
  DurationIn = 1.0f;
  Voiced = voiced;
  IsCancelled = false;
  SkipOnSkipMode = true;
  ProgressOnCancel = 0.0f;
  StartIn(true);
}

void TypewriterEffect::Update(float dt) {
  if (State == AnimationState::Stopped) return;

  if (CancelRequested && !IsCancelled) {
    IsCancelled = true;
    CancelRequested = false;
    ProgressOnCancel = Progress;
    DurationIn = 0.25f;
  }

  if (!IsCancelled) {
    if (Voiced && Profile::ConfigSystem::SyncVoice) {
      // Effectively progress at the constant pace such that the line ends
      // the moment the voice line ends
      const float remainingAudioDuration =
          Audio::Channels[Audio::AC_VOICE0]->DurationInSeconds() -
          Audio::Channels[Audio::AC_VOICE0]->PositionInSeconds();
      const float progressLeft = DurationIn - Progress;
      const float remainingAudioCompletionFraction =
          remainingAudioDuration > 0.0f ? dt / remainingAudioDuration : 1.0f;
      const float progressAdded =
          progressLeft * remainingAudioCompletionFraction;
      dt = progressAdded;
    } else {
      // Progress at the characters-per-second speed defined by TextSpeed
      const float progressLeft = 1.0f - Progress;

      if (Profile::ConfigSystem::TextSpeed >=
          Profile::ConfigSystem::TextSpeedBounds.y) {
        if (TextFadeInDuration > 0.0f) {
          dt = dt / TextFadeInDuration;
        } else {
          dt = progressLeft;
        }
      } else {
        const float glyphsLeft = static_cast<float>(GlyphCount) * progressLeft;
        const float secondsLeft =
            Profile::ConfigSystem::TextSpeed > 0.0f
                ? glyphsLeft / Profile::ConfigSystem::TextSpeed
                : 0.0f;
        const float secondsLeftFractionCompleted =
            secondsLeft > 0.0f ? dt / secondsLeft : 1.0f;
        const float progressAdded = progressLeft * secondsLeftFractionCompleted;
        dt = progressAdded;
      }
    }
  }

  UpdateImpl(dt);
}

TypewriterEffect::ParallelBlock TypewriterEffect::GetParallelBlock(
    const size_t glyph) {
  if (ParallelStartGlyphs.empty()) {  // No parallel blocks
    return {FirstGlyph, GlyphCount};

  } else if (glyph < *ParallelStartGlyphs.begin()) {  // First parallel block
    return {FirstGlyph, *ParallelStartGlyphs.begin() - FirstGlyph};

  } else if (glyph >= *ParallelStartGlyphs.rbegin()) {  // Last parallel block
    return {*ParallelStartGlyphs.rbegin(),
            (FirstGlyph + GlyphCount) - *ParallelStartGlyphs.rbegin()};

  } else {
    const size_t firstGlyphOfBlock = *std::max_element(
        ParallelStartGlyphs.begin(), ParallelStartGlyphs.upper_bound(glyph));
    return {firstGlyphOfBlock,
            *ParallelStartGlyphs.upper_bound(glyph) - firstGlyphOfBlock};
  }
}

std::pair<float, float> TypewriterEffect::GetGlyphWritingProgresses(
    const size_t glyph) {
  const ParallelBlock block = GetParallelBlock(glyph);

  const size_t parallelBlockGlyphNo = glyph - block.Start;

  // We start displaying a glyph after the previous one is 25% opaque, hence
  // totalDisplayTime = glyphCount * (0.25 * glyphDisplayTime) +
  //                    glyphDisplayTime * 0.75

  constexpr float singleGlyphDuration = 1.0f;
  constexpr float glyphPropagateProgress = 0.25f;
  constexpr float glyphPropagateDuration =
      singleGlyphDuration * glyphPropagateProgress;
  const float totalDuration =
      block.Size * glyphPropagateDuration +
      (1.0f - glyphPropagateProgress) * singleGlyphDuration;

  const float startTime = glyphPropagateDuration * parallelBlockGlyphNo;
  const float endTime = startTime + singleGlyphDuration;

  return {startTime / totalDuration, endTime / totalDuration};
}

float TypewriterEffect::CalcOpacity(size_t glyph) {
  if (glyph < FirstGlyph) return 1.0f;
  if (glyph >= FirstGlyph + GlyphCount) return 0.0f;

  if (!IsCancelled &&
      Profile::ConfigSystem::TextSpeed >=
          Profile::ConfigSystem::TextSpeedBounds.y &&
      !(Voiced && Profile::ConfigSystem::SyncVoice)) {
    return Progress;
  }

  const auto [startProgress, endProgress] = GetGlyphWritingProgresses(glyph);

  if (IsCancelled) {
    // On cancellation, all non-opaque glyphs start appearing simultaneously

    // Opaque glyphs remain opaque
    if (ProgressOnCancel >= endProgress) return 1.0f;

    // Transparent glyphs fade in according to the progress made
    // since cancelling
    const float cancelProgress =
        (Progress - ProgressOnCancel) / (1.0f - ProgressOnCancel);
    if (ProgressOnCancel <= startProgress) return cancelProgress;

    // Translucent glyphs fade in further, in addition to the opacity they
    // already had
    const float glyphProgressBeforeCancellation =
        (ProgressOnCancel - startProgress) / (endProgress - startProgress);
    return glyphProgressBeforeCancellation +
           cancelProgress * (1.0f - glyphProgressBeforeCancellation);
  }

  return std::clamp((Progress - startProgress) / (endProgress - startProgress),
                    0.0f, 1.0f);
}

float TypewriterEffect::CalcRubyOpacity(const size_t rubyGlyphId,
                                        const RubyChunk& chunk) {
  // Ruby fade as a regular textbox would, with the start and end times of
  // the animation being the start and end fade times of the ruby chunk's base
  //
  // On cancellation, the ruby all fade in simultaneously, like regular text

  const size_t baseStart = chunk.FirstBaseCharacter;
  const size_t baseEnd = chunk.FirstBaseCharacter + chunk.BaseLength;

  // Base is already fully opaque / still fully transparent
  if (baseEnd <= FirstGlyph) return 1.0f;
  if (baseStart > FirstGlyph + GlyphCount) return 0.0f;

  const float baseStartProgress = GetGlyphWritingProgresses(baseStart).first;
  const float baseEndProgress = GetGlyphWritingProgresses(baseEnd - 1).second;

  if (IsCancelled) {
    if (ProgressOnCancel >= baseEndProgress) return 1.0f;

    const float cancelProgress =
        (Progress - ProgressOnCancel) / (1.0f - ProgressOnCancel);
    if (ProgressOnCancel <= baseStartProgress) return cancelProgress;
  } else {
    if (Progress >= baseEndProgress) return 1.0f;
    if (Progress <= baseStartProgress) return 0.0f;
  }

  const float baseProgressLength = baseEndProgress - baseStartProgress;

  // We start displaying a glyph after the previous one is 25% opaque, hence
  // totalDisplayTime = glyphCount * (0.25 * glyphDisplayTime) +
  //                    glyphDisplayTime * 0.75

  constexpr float singleGlyphDuration = 1.0f;
  constexpr float glyphPropagateProgress = 0.25f;
  constexpr float glyphPropagateDuration =
      singleGlyphDuration * glyphPropagateProgress;
  const float totalDuration =
      chunk.Length * glyphPropagateDuration +
      (1.0f - glyphPropagateProgress) * singleGlyphDuration;

  const float glyphStartTime = glyphPropagateDuration * rubyGlyphId;
  const float glyphEndTime = glyphStartTime + singleGlyphDuration;

  // Convert back to progress-space
  const float startProgress =
      baseStartProgress + (glyphStartTime / totalDuration) * baseProgressLength;
  const float endProgress =
      baseStartProgress + (glyphEndTime / totalDuration) * baseProgressLength;

  if (IsCancelled) {
    if (ProgressOnCancel >= endProgress) return 1.0f;

    const float cancelProgress =
        (Progress - ProgressOnCancel) / (1.0f - ProgressOnCancel);
    if (ProgressOnCancel <= startProgress) return cancelProgress;

    const float glyphProgressBeforeCancellation =
        (ProgressOnCancel - startProgress) / (endProgress - startProgress);
    return glyphProgressBeforeCancellation +
           cancelProgress * (1.0f - glyphProgressBeforeCancellation);
  }

  return std::clamp((Progress - startProgress) / (endProgress - startProgress),
                    0.0f, 1.0f);
}

bool DialoguePage::TextIsFullyOpaque() { return Typewriter.Progress == 1.0f; }

void DialoguePage::Init() {
  Profile::Dialogue::Configure();

  switch (DialogueBoxCurrentType) {
    case DialogueBoxType::MO6TW:
      TextBox = new MO6TW::DialogueBox();
      break;
    case DialogueBoxType::CHLCC:
      TextBox = new CHLCC::DialogueBox();
      break;
    case DialogueBoxType::CC:
      TextBox = new CC::DialogueBox();
      break;
    default:
      TextBox = new DialogueBox();
      break;
  }

  TextBox->Init();

  WaitIconDisplay::Init();
  AutoIconDisplay::Init();
  SkipIconDisplay::Init();
  NametagDisplay::Init();

  for (int i = 0; i < std::ssize(DialoguePages); i++) {
    DialoguePages[i].Clear();
    DialoguePages[i].Mode = DPM_NVL;
    DialoguePages[i].Id = i;
    DialoguePages[i].AnimationId = 0;
    DialoguePages[i].FadeAnimation.DurationIn = FadeInDuration;
    DialoguePages[i].FadeAnimation.DurationOut = FadeOutDuration;
    DialoguePages[i].FadeAnimation.SkipOnSkipMode = true;

    DialoguePages[i].TextFadeAnimation.DurationIn = TextFadeInDuration;
    DialoguePages[i].TextFadeAnimation.DurationOut = TextFadeOutDuration;
    DialoguePages[i].TextFadeAnimation.SkipOnSkipMode = true;
    DialoguePages[i].TextFadeAnimation.Progress = 1.0f;
  }
}

void DialoguePage::Clear() {
  Glyphs.clear();
  NameLength = 0;
  Name.clear();
  NameId.reset();
  std::fill(RubyChunks.begin(), RubyChunks.end(), RubyChunk{});
  RubyChunkCount = 0;
  CurrentRubyChunk = 0;
  FirstRubyChunkOnLine = 0;
  if (Mode == DPM_ADV) {
    CurrentLineTop = ADVBounds.Y;
  } else if (Mode == DPM_REV) {
    if (ScrWork[SW_MESWIN0TYPE] == 0) {
      CurrentLineTop = REVBounds.Y;
    } else {
      CurrentLineTop = SecondaryREVBounds.Y;
    }
  } else if (Mode == DPM_TIPS) {
    CurrentLineTop = TipsBounds.Y;
  } else {
    CurrentLineTop = NVLBounds.Y;
  }
  CurrentLineTopMargin = 0.0f;
  NVLResetBeforeAdd = false;
}

enum TextParseState { TPS_Normal, TPS_Name, TPS_Ruby };

void DialoguePage::FinishLine(Vm::Sc3VmThread* ctx, size_t nextLineStart,
                              const RectF& boxBounds, TextAlignment alignment) {
  // Lay out all ruby chunks on this line (before we change CurrentLineTop and
  // thus can't find where to put them)
  for (size_t i = FirstRubyChunkOnLine; i < RubyChunkCount; i++) {
    RubyChunk& chunk = RubyChunks[i];

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
  // Erin DialogueBox
  if (DialogueBoxCurrentType == +DialogueBoxType::CHLCC && Mode == DPM_REV &&
      ScrWork[SW_MESWIN0TYPE] == 1) {
    lineHeight = Impacto::Profile::CHLCC::DialogueBox::REVLineHeight;
  }
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
      Glyphs[LastLineStart].DestRect.X > boxBounds.X) {
    marginXOffset = (Glyphs[LastLineStart].DestRect.X - boxBounds.X) *
                    ((FontSize / DefaultFontSize) - 1.0f);
  }
  for (size_t i = LastLineStart; i < nextLineStart; i++) {
    Glyphs[i].DestRect.Y = CurrentLineTop + CurrentLineTopMargin +
                           (lineHeight - Glyphs[i].DestRect.Height);
    float lastGlyphX = Glyphs[nextLineStart - 1].DestRect.X +
                       Glyphs[nextLineStart - 1].DestRect.Width;
    switch (alignment) {
      case TextAlignment::Center:
        Glyphs[i].DestRect.X +=
            (boxBounds.Width - (lastGlyphX - boxBounds.X)) / 2.0f;
        break;
      case TextAlignment::Right:
        Glyphs[i].DestRect.X += boxBounds.Width - lastGlyphX - marginXOffset;
        break;
      case TextAlignment::Left:
        Glyphs[i].DestRect.X += marginXOffset;
      default:
        break;
    }
  }
  float lineSpacing = DialogueFont->LineSpacing;
  // Erin DialogueBox
  if (DialogueBoxCurrentType == +DialogueBoxType::CHLCC && Mode == DPM_REV &&
      ScrWork[SW_MESWIN0TYPE] == 1) {
    lineSpacing = Impacto::Profile::CHLCC::DialogueBox::REVLineSpacing;
  }
  if (Mode == DPM_TIPS) lineSpacing = TipsLineSpacing;
  CurrentLineTop =
      CurrentLineTop + CurrentLineTopMargin + lineHeight + lineSpacing;
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

void DialoguePage::AddString(Vm::Sc3VmThread* ctx, Audio::AudioStream* voice,
                             bool acted, int animId, int charId,
                             bool shouldUpdateCharId) {
  CurrentVoice = voice;
  CurrentLineVoiced = voice != nullptr;
  ShouldUpdateCharId = shouldUpdateCharId;

  bool hasName = false;
  if (ShouldUpdateCharId) {
    CharacterId = charId;
  }

  const int nextAnimId = acted ? animId : charId;

  // Hold last voiced animation id
  if (CurrentLineVoiced) NextAnimationId = std::max(nextAnimId, 31);

  PrevNameId = NameId;

  if (Mode == DPM_ADV || Mode == DPM_REV || NVLResetBeforeAdd ||
      PrevMode != Mode) {
    Clear();
    TextFadeAnimation.Progress = 1.0f;
  }
  PrevMode = Mode;

  const size_t typeWriterStart = Glyphs.size();
  std::set<size_t> parallelStartGlyphs;

  // TODO should we reset HasName here?
  // It shouldn't really matter since names are an ADV thing and we clear
  // before every add on ADV anyway...

  AutoForward = AutoForwardType::Off;

  TextParseState state = TPS_Normal;
  // TODO respect alignment
  Alignment = TextAlignment::Left;
  size_t lastWordStart = Glyphs.size();
  LastLineStart = Glyphs.size();
  DialogueColorPair currentColors = ColorTable[0];
  if (Mode == DPM_REV) currentColors = ColorTable[REVColor];

  FontSize = DefaultFontSize;
  // Erin DialogueBox
  if (DialogueBoxCurrentType == +DialogueBoxType::CHLCC && Mode == DPM_REV &&
      ScrWork[SW_MESWIN0TYPE] == 1) {
    FontSize = Impacto::Profile::CHLCC::DialogueBox::REVFontSize;
  } else {
    FontSize = DefaultFontSize;
  }

  if (Mode == DPM_ADV) {
    BoxBounds = ADVBounds;
  } else if (Mode == DPM_REV) {
    if (ScrWork[SW_MESWIN0TYPE] == 0) {
      BoxBounds = REVBounds;
    } else {
      BoxBounds = SecondaryREVBounds;
    }
  } else if (Mode == DPM_TIPS) {
    BoxBounds = TipsBounds;
    currentColors = ColorTable[TipsColorIndex];
  } else {
    BoxBounds = NVLBounds;
  }

  float currentX = 0.0f;

  uint16_t name[DialogueMaxNameLength];

  BuildingRubyBase = false;

  StringToken token;
  bool prevGlyphWordStarting = false;
  bool prevGlyphWordEnding = false;
  do {
    token.Read(ctx);
    switch (token.Type) {
      case STT_LineBreak:
      case STT_AltLineBreak: {
        FinishLine(ctx, Glyphs.size(), BoxBounds, Alignment);
        lastWordStart = Glyphs.size();
        currentX = 0.0f;
      } break;

      case STT_CharacterNameStart: {
        hasName = true;
        Name.reserve(DialogueMaxNameLength);
        state = TPS_Name;
        if (Mode == DPM_REV &&
            REVNameLocation != +REVNameLocationType::LeftTop) {
          CurrentLineTop += REVNameOffset;
        }
        lastWordStart = Glyphs.size();
      } break;

      case STT_RubyTextStart: {
        EndRubyBase((int)Glyphs.size() - 1);
        state = TPS_Ruby;
      } break;

      case STT_RubyCenterPerCharacter: {
        RubyChunks[CurrentRubyChunk].CenterPerCharacter = true;
      } break;

      case STT_DialogueLineStart: {
        state = TPS_Normal;
        lastWordStart = Glyphs.size();
      } break;

      case STT_RubyTextEnd: {
        // At least S;G uses [ruby-base]link text[ruby-text-end] for mails,
        // with no ruby-text-start
        EndRubyBase((int)Glyphs.size() - 1);
        state = TPS_Normal;
        lastWordStart = Glyphs.size();
      } break;

      case STT_CenterText: {
        Alignment = TextAlignment::Center;
      } break;

      case STT_Present_Clear: {
        NVLResetBeforeAdd = true;
      } break;

      case STT_SetLeftMargin: {
        float addX = token.Val_Uint16;
        if (currentX + addX > BoxBounds.Width) {
          FinishLine(ctx, Glyphs.size(), BoxBounds, Alignment);
          addX -= (BoxBounds.Width - currentX);
          currentX = 0.0f;
        }
        while (addX > BoxBounds.Width) {
          FinishLine(ctx, Glyphs.size(), BoxBounds, Alignment);
          addX -= BoxBounds.Width;
        }
        currentX += addX;
        lastWordStart = Glyphs.size();
      } break;

      case STT_SetTopMargin: {
        CurrentLineTopMargin = token.Val_Uint16;
      } break;

      case STT_SetFontSize: {
        FontSize = DefaultFontSize * (token.Val_Uint16 / SetFontSizeRatio);
      } break;

      case STT_RubyBaseStart: {
        CurrentRubyChunk = static_cast<int>(RubyChunkCount);
        RubyChunkCount++;
        RubyChunks[CurrentRubyChunk].FirstBaseCharacter = Glyphs.size();
        BuildingRubyBase = true;
        lastWordStart = Glyphs.size();
      } break;

      case STT_AutoForward_SyncVoice: {
        AutoForward = AutoForwardType::SyncVoice;
      } break;

      case STT_AutoForward: {
        AutoForward = AutoForwardType::Normal;
      } break;

      case STT_SetColor: {
        if (Mode == DPM_REV) break;

        assert(token.Val_Expr < ColorCount);
        currentColors = ColorTable[token.Val_Expr];
      } break;

      case STT_UnlockTip: {
        if ((Mode == DPM_ADV ||
             (ScrWork[SW_MESWIN0TYPE] == 1 && Mode == DPM_REV) ||
             Mode == DPM_NVL) &&
            TipsSystem::GetTipLockedState(token.Val_Uint16)) {
          TipsSystem::SetTipLockedState(token.Val_Uint16, false);
          TipsNotification::AddTip(token.Val_Uint16);
          TipsSystem::GetNewTipsIndices().push_back(token.Val_Uint16);
        }
      } break;

      case STT_Character: {
        if (state == TPS_Name) {
          name[NameLength] = SDL_Swap16(token.Val_Uint16 | 0x8000);
          NameLength++;
        } else if (state == TPS_Ruby) {
          RubyChunks[CurrentRubyChunk]
              .RawText[RubyChunks[CurrentRubyChunk].Length] =
              SDL_Swap16(token.Val_Uint16 | 0x8000);
          RubyChunks[CurrentRubyChunk].Length++;
        } else {
          // TODO respect TA_Center
          // TODO what to do about left margin if text alignment is center?
          Glyphs.push_back(ProcessedTextGlyph());
          ProcessedTextGlyph& ptg = Glyphs.back();
          ptg.CharId = token.Val_Uint16;
          if (Mode == DPM_REV || Mode == DPM_TIPS)
            ptg.Opacity = 1.0f;
          else
            ptg.Opacity = 0.0f;
          ptg.Colors = currentColors;

          if (token.Flags & CharacterTypeFlags::WordStartingPunct) {
            // Ensure only the leftmost consecutive WordStartingPunct is counted
            if (!prevGlyphWordStarting) {
              // Still *before* this character
              lastWordStart = Glyphs.size() - 1;
              prevGlyphWordStarting = true;
            }
          } else {
            prevGlyphWordStarting = false;
          }

          if (token.Flags & CharacterTypeFlags::WordEndingPunct) {
            prevGlyphWordEnding = true;
            // Ensure only the rightmost consecutive WordEndingPunct is counted
          } else if (prevGlyphWordEnding) {
            // Previous character was word ending, so this character marks the
            // beginning of the next word
            lastWordStart = Glyphs.size() - 1;
            prevGlyphWordEnding = false;
          }

          ptg.DestRect.X = BoxBounds.X + currentX;
          ptg.DestRect.Width = (FontSize / DialogueFont->BitmapEmWidth) *
                               DialogueFont->AdvanceWidths[ptg.CharId];
          ptg.DestRect.Height = FontSize;

          currentX += ptg.DestRect.Width;

          // Line breaking
          if (ptg.DestRect.X + ptg.DestRect.Width >
              BoxBounds.X + BoxBounds.Width) {
            if (LastLineStart == lastWordStart) {
              // Word doesn't fit on a line, gotta break in the middle of it
              ptg.DestRect.X = BoxBounds.X;
              currentX = ptg.DestRect.Width;
              FinishLine(ctx, Glyphs.size() - 1, BoxBounds, Alignment);
              lastWordStart = Glyphs.size() - 1;
            } else {
              size_t firstNonSpace = lastWordStart;
              // Skip spaces at start of (new) line
              for (size_t i = lastWordStart; i < Glyphs.size(); i++) {
                const bool isSpace = StringToken::GetFlags(Glyphs[i].CharId) &
                                     CharacterTypeFlags::Space;
                if (!isSpace) break;
                firstNonSpace = i + 1;
              }

              FinishLine(ctx, firstNonSpace, BoxBounds, Alignment);
              lastWordStart = firstNonSpace;

              currentX = 0.0f;
              for (size_t i = firstNonSpace; i < Glyphs.size(); i++) {
                Glyphs[i].DestRect.X = BoxBounds.X + currentX;
                currentX += Glyphs[i].DestRect.Width;
              }
            }
          }
        }
      } break;

      case STT_PrintInParallel:
        parallelStartGlyphs.insert(Glyphs.size());
        break;

      default:
        break;
    }
  } while (token.Type != STT_EndOfString);

  FinishLine(ctx, Glyphs.size(), BoxBounds, Alignment);
  currentX = 0.0f;

  RectF boundingBox = Glyphs.empty() ? RectF() : Glyphs.begin()->DestRect;
  for (const ProcessedTextGlyph& glyph : Glyphs) {
    boundingBox = RectF::Coalesce(boundingBox, glyph.DestRect);
  }
  Dimensions = glm::vec2(boundingBox.Width, boundingBox.Height);

  // Even if there is a name in the string it should not be
  // rendered when in NVL mode
  if (Mode == DPM_NVL) {
    hasName = false;
    NameLength = 0;
  }

  if (DialogueBoxCurrentType == +DialogueBoxType::CHLCC && Mode == DPM_REV &&
      ScrWork[SW_MESWIN0TYPE] == 1) {
    hasName = false;
    NameLength = 0;
  }

  if (hasName) {
    NameId = GetNameId((uint8_t*)name, static_cast<int>(NameLength * 2));

    float fontSize = ADVNameFontSize;
    glm::vec2 pos = ADVNamePos;
    TextAlignment alignment = ADVNameAlignment;
    int colorIndex = 0;
    if (Mode == DPM_REV) {
      fontSize = REVNameFontSize;
      colorIndex = REVNameColor;

      switch (REVNameLocation) {
        case REVNameLocationType::None:
        case REVNameLocationType::TopLeft:
          pos = glm::vec2(REVBounds.X, REVBounds.Y);
          alignment = TextAlignment::Left;
          break;
        case REVNameLocationType::LeftTop:
          pos = glm::vec2(REVBounds.X - REVNameOffset, Glyphs[0].DestRect.Y);
          alignment = TextAlignment::Right;
          break;
      }
    }
    Vm::Sc3Stream nameStream(name);
    Name = TextLayoutPlainLine(nameStream, static_cast<int>(NameLength),
                               DialogueFont, fontSize, ColorTable[colorIndex],
                               1.0f, pos, alignment);
    assert(NameLength == Name.size());
  }

  ShouldShowNewText = true;
  // resetting typewriter for a new line and setting new params
  Typewriter.Stop();
  Typewriter.SetFirstGlyph(typeWriterStart);
  Typewriter.SetGlyphCount(Glyphs.size() - typeWriterStart);
  Typewriter.SetParallelStartGlyphs(parallelStartGlyphs);
  Typewriter.Progress = 0;

  NametagDisplay::UpdateNames(NameId, PrevNameId, Mode);
}

void DialoguePage::Update(float dt) {
  if (GetFlag(SF_UIHIDDEN)) return;
  if ((ScrWork[SW_GAMESTATE] & 4) != 0) return;

  // delayed start of a typewriter, voice & wait timer
  if (ShouldShowNewText && NametagDisplay::DialogueCanStartAppearing(Mode)) {
    ShouldShowNewText = false;
    if (CurrentLineVoiced) {
      AnimationId = NextAnimationId;
      Audio::Channels[Audio::AC_VOICE0]->Play(
          std::unique_ptr<Audio::AudioStream>(CurrentVoice), false, 0.0f);
    }

    if (ShouldUpdateCharId) {
      ScrWork[Id + SW_ANIME0CHANO] = CharacterId;
      ShouldUpdateCharId = false;
    }

    Typewriter.Start(CurrentLineVoiced);

    AutoWaitTime = static_cast<float>(Typewriter.GetGlyphCount());
    if (AutoForward == AutoForwardType::SyncVoice) AutoWaitTime *= 2.0f;
  }

  Typewriter.Update(dt);

  if (Typewriter.IsPlaying() || Typewriter.IsFinished(AnimationDirection::In)) {
    for (size_t i = 0; i < Glyphs.size(); i++) {
      Glyphs[i].Opacity = Typewriter.CalcOpacity(i);
    }

    for (size_t rubyChunkId = 0; rubyChunkId < RubyChunkCount; rubyChunkId++) {
      for (size_t rubyGlyphId = 0; rubyGlyphId < RubyChunks[rubyChunkId].Length;
           rubyGlyphId++) {
        RubyChunks[rubyChunkId].Text[rubyGlyphId].Opacity =
            Typewriter.CalcRubyOpacity(rubyGlyphId, RubyChunks[rubyChunkId]);
      }
    }

    if (AutoForward == AutoForwardType::SyncVoice) {
      const float speed = AutoWaitTime > Typewriter.GetGlyphCount()
                              ? Profile::ConfigSystem::TextSpeed
                              : Profile::ConfigSystem::AutoSpeed;
      AutoWaitTime = std::max(0.0f, AutoWaitTime - speed * dt);
    } else if (TextIsFullyOpaque() &&
               (AutoForward == AutoForwardType::Normal || AutoModeEnabled)) {
      AutoWaitTime =
          std::max(0.0f, AutoWaitTime - Profile::ConfigSystem::AutoSpeed * dt);
    }
  }

  TextBox->Update(dt);
  FadeAnimation.Update(dt);
  TextFadeAnimation.Update(dt);

  WaitIconDisplay::Update(dt);
  AutoIconDisplay::Update(dt);
  SkipIconDisplay::Update(dt);
  NametagDisplay::Update(dt, Mode);
}

void DialoguePage::Render() {
  // dialogue text
  if (GetFlag(SF_UIHIDDEN)) return;
  if (FadeAnimation.IsOut()) return;

  glm::vec4 opacityTint(1.0f);
  opacityTint.a = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);

  // Textbox
  float width = 0.0f;
  if (NameId.has_value()) {
    for (size_t i = 0; i < NameLength; i++) {
      width += Name[i].DestRect.Width;
    }
  }

  const std::optional<uint32_t> animatedName = NametagDisplay::GetNameToDraw();

  TextBox->Render(Mode, width, animatedName, opacityTint.a);

  // TODO: Figure out what's up with text box coloring
  glm::vec4 col = glm::vec4(1.0f);  // ScrWorkGetColor(SW_MESWINDOW_COLOR);
  col.a = opacityTint.a;

  const float textFadeOpacity =
      GetFlag(Profile::ScriptVars::SF_MESALLSKIP)
          ? 1.0f
          : opacityTint.a * TextFadeAnimation.Progress;

  Renderer->DrawProcessedText(Glyphs, DialogueFont, textFadeOpacity,
                              RendererOutlineMode::Full);
  for (size_t rubyChunkId = 0; rubyChunkId < RubyChunkCount; rubyChunkId++) {
    Renderer->DrawProcessedText(RubyChunks[rubyChunkId].Text, DialogueFont,
                                textFadeOpacity, RendererOutlineMode::Full);
  }

  if (NameId.has_value() && ADVBoxShowName) {
    float nameOpacity = (NametagCurrentType == +NametagType::Instant)
                            ? opacityTint.a
                            : textFadeOpacity;
    Renderer->DrawProcessedText(Name, DialogueFont, nameOpacity,
                                RendererOutlineMode::Full);
  }

  // Wait icon
  const RectF& lastGlyphDest =
      Glyphs.empty() ? RectF() : Glyphs.back().DestRect;
  glm::vec2 waitIconPos(lastGlyphDest.X + lastGlyphDest.Width, lastGlyphDest.Y);
  WaitIconDisplay::Render(waitIconPos, col, Mode);

  AutoIconDisplay::Render(col);
  SkipIconDisplay::Render(col);
}

void DialoguePage::Move(glm::vec2 relativePos) {
  for (ProcessedTextGlyph& glyph : Glyphs) {
    glyph.DestRect.X += relativePos.x;
    glyph.DestRect.Y += relativePos.y;
  }
  if (NameId.has_value()) {
    for (size_t i = 0; i < NameLength; i++) {
      Name[i].DestRect.X += relativePos.x;
      Name[i].DestRect.Y += relativePos.y;
    }
  }
  for (RubyChunk rubyChunk : std::span(RubyChunks.begin(), RubyChunkCount)) {
    for (auto glyph : std::span(rubyChunk.Text.begin(), rubyChunk.Length)) {
      glyph.DestRect.X += relativePos.x;
      glyph.DestRect.Y += relativePos.y;
    }
  }
}

void DialoguePage::MoveTo(glm::vec2 pos) {
  if (Glyphs.empty()) return;
  glm::vec2 relativePos =
      pos - glm::vec2(Glyphs[0].DestRect.X, Glyphs[0].DestRect.Y);
  Move(relativePos);
}

void DialoguePage::Hide() {
  FadeAnimation.StartOut();

  if (NameId.has_value()) {
    PrevNameId = NameId;
    NameId.reset();
    NametagDisplay::UpdateNames(NameId, PrevNameId, Mode);
  }
}

void DialoguePage::Show() {
  NametagDisplay::Reset();
  FadeAnimation.StartIn(true);
  TextFadeAnimation.Progress = 1.0f;
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
    uint32_t nameHash = GetHashCode(dummy.GetIp(), nameLength);
    NamePlateData[nameHash] = id;
  } while (stream.PeekU16() != 0xFFFF);
}

uint32_t GetNameId(uint8_t* name, int nameLength) {
  uint32_t nameHash = GetHashCode(name, nameLength);
  if (NamePlateData.find(nameHash) != NamePlateData.end())
    return NamePlateData[nameHash];
  else
    return 0;
}
}  // namespace Impacto