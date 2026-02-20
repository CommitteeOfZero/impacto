#include "dialoguepage.h"

#include "../profile/configsystem.h"
#include "../profile/dialogue.h"
#include "../profile/games/chlcc/dialoguebox.h"

#include "../data/tipssystem.h"

#include "../hud/waiticondisplay.h"
#include "../hud/autoicondisplay.h"
#include "../hud/skipicondisplay.h"
#include "../hud/nametagdisplay.h"
#include "../hud/dialoguebox.h"
#include "../hud/tipsnotification.h"

#include "../games/mo6tw/dialoguebox.h"
#include "../games/chlcc/dialoguebox.h"
#include "../games/cc/dialoguebox.h"

#include "../audio/audiostream.h"
#include "../audio/audiosystem.h"
#include "../audio/audiostream.h"
#include "../audio/audiochannel.h"

namespace Impacto {
using namespace Impacto::Profile::Dialogue;
using namespace Impacto::Profile::ScriptVars;

DialoguePage::State DialoguePage::GetState() const {
  using enum State;

  if (Typewriter.IsOut()) return Initial;
  if (Typewriter.IsPlaying()) return Showing;
  // Typewriter is in

  if (TextFadeAnimation.IsIn()) return Shown;
  if (TextFadeAnimation.IsPlaying()) return Hiding;
  // Text fade animation is out

  return Hidden;
}

bool DialoguePage::TextIsFullyOpaque() { return Typewriter.Progress == 1.0f; }

void DialoguePage::Init() {
  Profile::Dialogue::Configure();

  WaitIconDisplay::Init();
  AutoIconDisplay::Init();
  SkipIconDisplay::Init();

  DialoguePages.resize(PageCount);
  for (DialoguePage& page : DialoguePages) {
    page.Glyphs.reserve(MaxPageSize);
  }

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
  ClearName();
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
  AdvanceMethod = AdvanceMethodType::Skip;
}

void DialoguePage::ClearName() {
  Name.clear();
  RenderName = false;
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

  bool hasName = false;
  if (shouldUpdateCharId) {
    CharacterId = charId;
    ScrWork[Id + SW_ANIME0CHANO] = CharacterId;
  }

  const int nextAnimId = acted ? animId : charId;

  // Hold last voiced animation id
  if (CurrentLineVoiced) NextAnimationId = std::max(nextAnimId, 31);

  if (Mode == DPM_ADV || Mode == DPM_REV ||
      AdvanceMethod == AdvanceMethodType::PresentClear || PrevMode != Mode) {
    Clear();
  }
  TextFadeAnimation.Reset(AnimationDirection::Out);
  PrevMode = Mode;

  const size_t typeWriterStart = Glyphs.size();
  std::set<size_t> parallelStartGlyphs;

  // TODO should we reset HasName here?
  // It shouldn't really matter since names are an ADV thing and we clear
  // before every add on ADV anyway...

  AdvanceMethod = AdvanceMethodType::Skip;

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

  std::vector<uint16_t> name;

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
        name.reserve(64);
        state = TPS_Name;
        if (Mode == DPM_REV &&
            REVNameLocation != +REVNameLocationType::LeftTop) {
          CurrentLineTop += REVNameOffset;
        }
        lastWordStart = Glyphs.size();
      } break;

      case STT_Present: {
        AdvanceMethod = AdvanceMethodType::Present;
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
        AdvanceMethod = AdvanceMethodType::PresentClear;
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

      case STT_Present_0x18: {
        AdvanceMethod = AdvanceMethodType::Present0x18;
      } break;

      case STT_AutoForward_SyncVoice: {
        AdvanceMethod = AdvanceMethodType::AutoForwardSyncVoice;
      } break;

      case STT_AutoForward: {
        AdvanceMethod = AdvanceMethodType::AutoForward;
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
          name.emplace_back(SDL_Swap16(token.Val_Uint16 | 0x8000));
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
  }

  if (DialogueBoxCurrentType == +DialogueBoxType::CHLCC && Mode == DPM_REV &&
      ScrWork[SW_MESWIN0TYPE] == 1) {
    hasName = false;
  }

  RenderName = hasName;
  if (hasName) {
    ScrWork[SW_MESNAMEID0 + Id] = GetNameId(name).value_or(0xFFFF);

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
    Vm::Sc3Stream nameStream(name.data());
    Name = TextLayoutPlainLine(nameStream, static_cast<int>(name.size()),
                               DialogueFont, fontSize, ColorTable[colorIndex],
                               1.0f, pos, alignment);
    assert(name.size() == Name.size());
  } else {
    ScrWork[SW_MESNAMEID0 + Id] = 0xFFFF;
  }

  // resetting typewriter for a new line and setting new params
  Typewriter.Reset(AnimationDirection::In);
  Typewriter.SetFirstGlyph(typeWriterStart);
  Typewriter.SetGlyphCount(Glyphs.size() - typeWriterStart);
  Typewriter.SetParallelStartGlyphs(parallelStartGlyphs);
  Typewriter.Start(CurrentLineVoiced);

  if (CurrentLineVoiced) {
    AnimationId = NextAnimationId;
    Audio::Channels[Audio::AC_VOICE0]->Play(
        std::unique_ptr<Audio::AudioStream>(CurrentVoice), false, 0.0f);
  }

  AutoWaitTime = static_cast<float>(Typewriter.GetGlyphCount());
  if (AdvanceMethod == AdvanceMethodType::AutoForwardSyncVoice) {
    AutoWaitTime *= 2.0f;
  }
}

void DialoguePage::Update(float dt) {
  if (GetFlag(SF_UIHIDDEN)) return;
  if ((ScrWork[SW_GAMESTATE] & 4) != 0) return;

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

    if (AdvanceMethod == AdvanceMethodType::AutoForwardSyncVoice) {
      const float speed = AutoWaitTime > Typewriter.GetGlyphCount()
                              ? Profile::ConfigSystem::TextSpeed
                              : Profile::ConfigSystem::AutoSpeed;
      AutoWaitTime = std::max(0.0f, AutoWaitTime - speed * dt);
    } else if (TextIsFullyOpaque() &&
               (AdvanceMethod == AdvanceMethodType::AutoForward ||
                AutoModeEnabled)) {
      AutoWaitTime =
          std::max(0.0f, AutoWaitTime - Profile::ConfigSystem::AutoSpeed * dt);
    }
  }

  const State visibilityState = GetState();
  const bool shouldHide =
      visibilityState == State::Hiding || visibilityState == State::Hidden;
  if (RenderName && !shouldHide) {
    DialogueBoxInst->ShowName();
  } else {
    DialogueBoxInst->HideName();
  }

  DialogueBoxInst->Update(dt);
  FadeAnimation.Update(dt);
  TextFadeAnimation.Update(dt);

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

  const NameInfo nameInfo{
      .RenderWindow = RenderName,
      .NameId = ScrWork[SW_MESNAMEID0 + Id] == 0xffff
                    ? std::nullopt
                    : std::optional(ScrWork[SW_MESNAMEID0 + Id]),
      .Name = Name,
  };
  DialogueBoxInst->Render(Mode, nameInfo, opacityTint);

  // TODO: Figure out what's up with text box coloring
  glm::vec4 col = glm::vec4(1.0f);  // ScrWorkGetColor(SW_MESWINDOW_COLOR);
  col.a = opacityTint.a;

  const float textFadeOpacity =
      GetFlag(SF_MESALLSKIP) ? 1.0f
                             : opacityTint.a * TextFadeAnimation.Progress;
  Renderer->DrawProcessedText(Glyphs, DialogueFont, textFadeOpacity,
                              RendererOutlineMode::Full);
  for (size_t rubyChunkId = 0; rubyChunkId < RubyChunkCount; rubyChunkId++) {
    Renderer->DrawProcessedText(RubyChunks[rubyChunkId].Text, DialogueFont,
                                textFadeOpacity, RendererOutlineMode::Full);
  }

  // Wait icon
  const RectF& lastGlyphDest =
      Glyphs.empty() ? RectF() : Glyphs.back().DestRect;
  glm::vec2 waitIconPos(lastGlyphDest.X + lastGlyphDest.Width, lastGlyphDest.Y);
  WaitIconDisplay::Render(waitIconPos, col, Mode, Id);

  AutoIconDisplay::Render(col);
  SkipIconDisplay::Render(col);
}

void DialoguePage::Move(glm::vec2 relativePos) {
  for (ProcessedTextGlyph& glyph : Glyphs) {
    glyph.DestRect.X += relativePos.x;
    glyph.DestRect.Y += relativePos.y;
  }
  for (ProcessedTextGlyph& glyph : Name) {
    glyph.DestRect.X += relativePos.x;
    glyph.DestRect.Y += relativePos.y;
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

void DialoguePage::Hide() { FadeAnimation.StartOut(); }

void DialoguePage::Show() {
  FadeAnimation.StartIn(true);
  TextFadeAnimation.Progress = 1.0f;
}
}  // namespace Impacto