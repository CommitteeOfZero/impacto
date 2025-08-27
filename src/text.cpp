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
  STT_UnlockTip = 0x16,
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
  int Read(Vm::Sc3Stream& stream);
};

int StringToken::Read(Vm::Sc3VmThread* ctx) {
  int bytesRead = 0;

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
        Vm::ExpressionEval(ctx, &Val_Expr);
        bytesRead += (int)(ctx->IpOffset - oldIp);
      }
      break;
    }

    case STT_EvaluateExpression: {
      Type = (StringTokenType)c;
      uint32_t oldIp = ctx->IpOffset;
      // TODO is this really okay to do in parsing code?
      Vm::ExpressionEval(ctx, &Val_Expr);
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

uint8_t ProcessedTextGlyph::Flags() const {
  return Profile::Charset::Flags[CharId];
}

void TypewriterEffect::Start(size_t firstGlyph, size_t glyphCount,
                             const std::set<size_t>& parallelStartGlyphs,
                             const bool voiced) {
  DurationIn = 1.0f;
  FirstGlyph = firstGlyph;
  GlyphCount = glyphCount;
  Voiced = voiced;
  IsCancelled = false;
  SkipOnSkipMode = true;
  ProgressOnCancel = 0.0f;
  ParallelStartGlyphs = parallelStartGlyphs;
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

  UpdateImpl(dt);
}

float TypewriterEffect::CalcOpacity(size_t glyph) {
  if (glyph < FirstGlyph) return 1.0f;
  if (glyph >= FirstGlyph + GlyphCount) return 0.0f;

  size_t parallelBlockFirstGlyph;
  size_t parallelBlockGlyphCount;
  if (ParallelStartGlyphs.empty()) {  // No parallel blocks
    parallelBlockFirstGlyph = FirstGlyph;
    parallelBlockGlyphCount = GlyphCount;

  } else if (glyph < *ParallelStartGlyphs.begin()) {  // First parallel block
    parallelBlockFirstGlyph = FirstGlyph;
    parallelBlockGlyphCount =
        *ParallelStartGlyphs.begin() - parallelBlockFirstGlyph;

  } else if (glyph >= *ParallelStartGlyphs.rbegin()) {  // Last parallel block
    parallelBlockFirstGlyph = *ParallelStartGlyphs.rbegin();
    parallelBlockGlyphCount =
        (FirstGlyph + GlyphCount) - *ParallelStartGlyphs.rbegin();

  } else {
    parallelBlockFirstGlyph = *std::max_element(
        ParallelStartGlyphs.begin(), ParallelStartGlyphs.upper_bound(glyph));
    parallelBlockGlyphCount =
        *ParallelStartGlyphs.upper_bound(glyph) - parallelBlockFirstGlyph;
  }

  const size_t parallelBlockGlyphNo = glyph - parallelBlockFirstGlyph;
  const float nonCancelledProgress = IsCancelled ? ProgressOnCancel : Progress;

  // We start displaying a glyph after the previous one is 25% opaque, hence
  // totalDisplayTime = glyphCount * (0.25 * glyphDisplayTime) +
  //                    glyphDisplayTime * 0.75

  constexpr float singleGlyphDuration = 1.0f;
  constexpr float glyphPropagateDuration = singleGlyphDuration * 0.25f;
  const float totalDuration = parallelBlockGlyphCount * glyphPropagateDuration +
                              0.75f * singleGlyphDuration;
  const float normalizedProgress = nonCancelledProgress * totalDuration;

  const float currentGlyphStart = glyphPropagateDuration * parallelBlockGlyphNo;
  const float currentGlyphEnd = currentGlyphStart + singleGlyphDuration;
  if (currentGlyphEnd <= normalizedProgress) return 1.0f;

  if (IsCancelled) {
    const float cancelProgress =
        (Progress - ProgressOnCancel) / (1.0f - ProgressOnCancel);
    if (normalizedProgress <= currentGlyphStart) return cancelProgress;

    const float nonCancelledGlyphProgress =
        (normalizedProgress - currentGlyphStart) / singleGlyphDuration;
    return nonCancelledGlyphProgress +
           cancelProgress * (1.0f - nonCancelledGlyphProgress);
  }

  if (normalizedProgress <= currentGlyphStart) return 0.0f;

  return (normalizedProgress - currentGlyphStart) / singleGlyphDuration;
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

  for (int i = 0; i < Profile::Dialogue::PageCount; i++) {
    DialoguePages[i].Clear();
    DialoguePages[i].Mode = DPM_NVL;
    DialoguePages[i].Id = i;
    DialoguePages[i].AnimationId = 0;
    DialoguePages[i].FadeAnimation.DurationIn = FadeInDuration;
    DialoguePages[i].FadeAnimation.DurationOut = FadeOutDuration;
    DialoguePages[i].FadeAnimation.SkipOnSkipMode = true;
  }
}

void DialoguePage::Clear() {
  Glyphs.clear();
  NameLength = 0;
  Name.clear();
  HasName = false;
  std::fill(RubyChunks.begin(), RubyChunks.end(), RubyChunk{});
  RubyChunkCount = 0;
  CurrentRubyChunk = 0;
  FirstRubyChunkOnLine = 0;
  if (Mode == DPM_ADV) {
    CurrentLineTop = ADVBounds.Y;
  } else if (Mode == DPM_REV) {
    CurrentLineTop = REVBounds.Y;
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
  EndRubyBase(static_cast<int>(nextLineStart) - 1);

  // Lay out all ruby chunks on this line (before we change CurrentLineTop and
  // thus can't find where to put them)
  for (size_t i = FirstRubyChunkOnLine; i < RubyChunkCount; i++) {
    if (RubyChunks[i].FirstBaseCharacter >= nextLineStart) break;

    Vm::Sc3Stream rubyText(RubyChunks[i].RawText);

    glm::vec2 pos =
        glm::vec2(0, CurrentLineTop + CurrentLineTopMargin + RubyYOffset);

    // ruby base length > ruby text length: block align
    // ruby base length > ruby text length and 0x1E: center per character
    // ruby base length == ruby text length: center per character
    // ruby base length < ruby text length: center over block (handled by
    // block align)

    if (RubyChunks[i].Length == RubyChunks[i].BaseLength ||
        (RubyChunks[i].CenterPerCharacter &&
         RubyChunks[i].BaseLength > RubyChunks[i].Length)) {
      // center every ruby character over the base character below it
      for (size_t j = 0; j < RubyChunks[i].Length; j++) {
        RectF const& baseGlyphRect =
            Glyphs[RubyChunks[i].FirstBaseCharacter + j].DestRect;
        pos.x = baseGlyphRect.Center().x;
        TextLayoutPlainLine(rubyText, 1, std::span(RubyChunks[i].Text + j, 1),
                            DialogueFont, RubyFontSize, ColorTable[0], 1.0f,
                            pos, TextAlignment::Center);
      }
    } else {
      // evenly space out all ruby characters over the block of base text
      // TODO is this really the right behaviour for CenterPerCharacter(0x1E)
      // and ruby base length < ruby text length?
      RectF const& baseGlyphRect =
          Glyphs[RubyChunks[i].FirstBaseCharacter].DestRect;
      pos.x = baseGlyphRect.X;
      /*
       * This seems unused right now
      float blockWidth = 0.0f;
      for (int j = 0; j < RubyChunks[i].BaseLength; j++) {
        blockWidth +=
            Glyphs[RubyChunks[i].FirstBaseCharacter + j].DestRect.Width;
      }
    int rubyLength =
        TextLayoutPlainLine(ctx, RubyChunks[i].Length, RubyChunks[i].Text,
                            DialogueFont, RubyFontSize, ColorTable[0], 1.0f,
                            pos, TextAlignment::Block, blockWidth);
    */
    }
    FirstRubyChunkOnLine++;
  }

  float lineHeight = FontSize;
  // Erin DialogueBox
  if (DialogueBoxCurrentType == +DialogueBoxType::CHLCC && Mode == DPM_REV) {
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
  if (DialogueBoxCurrentType == +DialogueBoxType::CHLCC && Mode == DPM_REV) {
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
                             int animId) {
  AnimationId = animId;
  if (Mode == DPM_ADV || Mode == DPM_REV || NVLResetBeforeAdd ||
      PrevMode != Mode) {
    Clear();
  }
  PrevMode = Mode;

  size_t typewriterStart = Glyphs.size();
  std::set<size_t> parallelStartGlyphs;

  // TODO should we reset HasName here?
  // It shouldn't really matter since names are an ADV thing and we clear
  // before every add on ADV anyway...

  AutoForward = false;

  TextParseState State = TPS_Normal;
  // TODO respect alignment
  Alignment = TextAlignment::Left;
  size_t LastWordStart = Glyphs.size();
  LastLineStart = Glyphs.size();
  DialogueColorPair CurrentColors = ColorTable[0];
  if (Mode == DPM_REV) CurrentColors = ColorTable[REVColor];

  FontSize = DefaultFontSize;
  // Erin DialogueBox
  if (DialogueBoxCurrentType == +DialogueBoxType::CHLCC && Mode == DPM_REV) {
    FontSize = Impacto::Profile::CHLCC::DialogueBox::REVFontSize;
  }

  if (Mode == DPM_ADV) {
    BoxBounds = ADVBounds;
  } else if (Mode == DPM_REV) {
    BoxBounds = REVBounds;
  } else if (Mode == DPM_TIPS) {
    BoxBounds = TipsBounds;
    CurrentColors = ColorTable[TipsColorIndex];
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
        FinishLine(ctx, Glyphs.size(), BoxBounds, Alignment);
        LastWordStart = Glyphs.size();
        CurrentX = 0.0f;
        break;
      }
      case STT_CharacterNameStart: {
        HasName = true;
        Name.reserve(DialogueMaxNameLength);
        State = TPS_Name;
        if (DialogueBoxCurrentType != +DialogueBoxType::CHLCC &&
            Mode == DPM_REV &&
            REVNameLocation != +REVNameLocationType::LeftTop) {
          CurrentLineTop += REVNameOffset;
        }
        break;
      }
      case STT_RubyTextStart: {
        EndRubyBase((int)Glyphs.size() - 1);
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
        // At least S;G uses [ruby-base]link text[ruby-text-end] for mails,
        // with no ruby-text-start
        EndRubyBase((int)Glyphs.size() - 1);
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
          FinishLine(ctx, Glyphs.size(), BoxBounds, Alignment);
          LastWordStart = Glyphs.size();
          addX -= (BoxBounds.Width - CurrentX);
          CurrentX = 0.0f;
        }
        while (addX > BoxBounds.Width) {
          FinishLine(ctx, Glyphs.size(), BoxBounds, Alignment);
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
        FontSize = DefaultFontSize * (token.Val_Uint16 / SetFontSizeRatio);
        break;
      }
      case STT_RubyBaseStart: {
        CurrentRubyChunk = static_cast<int>(RubyChunkCount);
        RubyChunkCount++;
        RubyChunks[CurrentRubyChunk].FirstBaseCharacter = Glyphs.size();
        LastWordStart = Glyphs.size();
        break;
      }
      case STT_AutoForward:
      case STT_AutoForward_1A: {
        AutoForward = true;
        break;
      }
      case STT_SetColor: {
        if (Mode == DPM_REV) break;

        assert(token.Val_Expr < ColorCount);
        CurrentColors = ColorTable[token.Val_Expr];
        break;
      }
      case STT_UnlockTip: {
        if ((Mode == DPM_ADV ||
             (DialogueBoxCurrentType == +DialogueBoxType::CHLCC &&
              Mode == DPM_REV) ||
             Mode == DPM_NVL) &&
            TipsSystem::GetTipLockedState(token.Val_Uint16)) {
          TipsSystem::SetTipLockedState(token.Val_Uint16, false);
          TipsNotification::AddTip(token.Val_Uint16);
        }
        break;
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
          Glyphs.push_back(ProcessedTextGlyph());
          ProcessedTextGlyph& ptg = Glyphs.back();
          ptg.CharId = token.Val_Uint16;
          if (Mode == DPM_REV || Mode == DPM_TIPS)
            ptg.Opacity = 1.0f;
          else
            ptg.Opacity = 0.0f;
          ptg.Colors = CurrentColors;

          if (ptg.Flags() & CharacterTypeFlags::WordStartingPunct) {
            LastWordStart = Glyphs.size();  // still *before* this character
          }

          ptg.DestRect.X = BoxBounds.X + CurrentX;
          ptg.DestRect.Width = (FontSize / DialogueFont->BitmapEmWidth) *
                               DialogueFont->AdvanceWidths[ptg.CharId];
          ptg.DestRect.Height = FontSize;

          CurrentX += ptg.DestRect.Width;

          // Line breaking
          if (ptg.DestRect.X + ptg.DestRect.Width >
              BoxBounds.X + BoxBounds.Width) {
            if (LastLineStart == LastWordStart) {
              // Word doesn't fit on a line, gotta break in the middle of it
              ptg.DestRect.X = BoxBounds.X;
              CurrentX = ptg.DestRect.Width;
              FinishLine(ctx, Glyphs.size() - 1, BoxBounds, Alignment);
              LastWordStart = Glyphs.size() - 1;
            } else {
              size_t firstNonSpace = LastWordStart;
              // Skip spaces at start of (new) line
              for (size_t i = LastWordStart; i < Glyphs.size(); i++) {
                if (!(Glyphs[i].Flags() & CharacterTypeFlags::Space)) break;
                firstNonSpace = i + 1;
              }
              FinishLine(ctx, firstNonSpace, BoxBounds, Alignment);
              LastWordStart = firstNonSpace;
              CurrentX = 0.0f;
              for (size_t i = firstNonSpace; i < Glyphs.size(); i++) {
                Glyphs[i].DestRect.X = BoxBounds.X + CurrentX;
                CurrentX += Glyphs[i].DestRect.Width;
              }
            }
          }

          if (ptg.Flags() & CharacterTypeFlags::WordEndingPunct) {
            LastWordStart = Glyphs.size();  // now after this character
          }
        }
      } break;

      case STT_PrintInParallel:
        parallelStartGlyphs.insert(Glyphs.size());
        break;

      default: {
        break;
      }
    }
  } while (token.Type != STT_EndOfString);

  FinishLine(ctx, Glyphs.size(), BoxBounds, Alignment);
  CurrentX = 0.0f;

  RectF boundingBox = Glyphs.empty() ? RectF() : Glyphs.begin()->DestRect;
  for (const ProcessedTextGlyph& glyph : Glyphs) {
    boundingBox = RectF::Coalesce(boundingBox, glyph.DestRect);
  }
  Dimensions = glm::vec2(boundingBox.Width, boundingBox.Height);

  // Even if there is a name in the string it should not be
  // rendered when in NVL mode
  if (Mode == DPM_NVL) {
    HasName = false;
    NameLength = 0;
  }

  if (DialogueBoxCurrentType == +DialogueBoxType::CHLCC && Mode == DPM_REV) {
    HasName = false;
    NameLength = 0;
  }

  if (HasName) {
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

  if (voice != 0) {
    Audio::Channels[Audio::AC_VOICE0]->Play(
        std::unique_ptr<Audio::AudioStream>(voice), false, 0.0f);
  }

  Typewriter.Start(typewriterStart, Glyphs.size() - typewriterStart,
                   parallelStartGlyphs, voice != nullptr);
}

void DialoguePage::Update(float dt) {
  if (GetFlag(SF_UIHIDDEN)) return;
  if ((ScrWork[SW_GAMESTATE] & 4) != 0) return;
  Typewriter.Update(dt);

  for (size_t i = 0; i < Glyphs.size(); i++) {
    Glyphs[i].Opacity = Typewriter.CalcOpacity(i);
  }

  if (TextIsFullyOpaque() && MesSkipMode & SkipModeFlags::Auto)
    AutoWaitTime =
        std::max(0.0f, AutoWaitTime - Profile::ConfigSystem::AutoSpeed * dt);

  TextBox->Update(dt);
  FadeAnimation.Update(dt);

  WaitIconDisplay::Update(dt);
  AutoIconDisplay::Update(dt);
  SkipIconDisplay::Update(dt);
}

void DialoguePage::Render() {
  // dialogue text
  if (GetFlag(SF_UIHIDDEN)) return;
  if (FadeAnimation.IsOut()) return;

  glm::vec4 opacityTint(1.0f);
  opacityTint.a = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);

  // Textbox
  float width = 0.0f;
  if (HasName) {
    for (size_t i = 0; i < NameLength; i++) {
      width += Name[i].DestRect.Width;
    }
  }

  TextBox->Render(Mode, HasName, width, NameId, opacityTint.a);

  // TODO: Figure out what's up with text box coloring
  glm::vec4 col = glm::vec4(1.0f);  // ScrWorkGetColor(SW_MESWINDOW_COLOR);
  col.a = opacityTint.a;

  Renderer->DrawProcessedText(Glyphs, DialogueFont, opacityTint.a,
                              RendererOutlineMode::Full);

  if (HasName && ADVBoxShowName) {
    Renderer->DrawProcessedText(Name, DialogueFont, opacityTint.a,
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
  if (HasName) {
    for (size_t i = 0; i < NameLength; i++) {
      Name[i].DestRect.X += relativePos.x;
      Name[i].DestRect.Y += relativePos.y;
    }
  }
}

void DialoguePage::MoveTo(glm::vec2 pos) {
  glm::vec2 relativePos =
      pos - glm::vec2(Glyphs[0].DestRect.X, Glyphs[0].DestRect.Y);
  Move(relativePos);
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
  for (int i = 0; i < stringLength; i++) {
    token.Read(sc3);
    if (token.Type == STT_EndOfString) break;
    if (token.Type != STT_Character) continue;

    ProcessedTextGlyph ptg;
    ptg.CharId = token.Val_Uint16;
    ptg.Colors = colors;
    ptg.Opacity = opacity;

    ptg.DestRect.X = currentX;
    ptg.DestRect.Y = pos.y;
    ptg.DestRect.Width = std::floor((fontSize / font->BitmapEmWidth) *
                                    font->AdvanceWidths[ptg.CharId]);
    ptg.DestRect.Height = fontSize;

    currentX += ptg.DestRect.Width;

    *outIt++ = ptg;
    characterCount++;
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