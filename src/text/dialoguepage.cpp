#include "dialoguepage.h"

#include "textparser.h"

#include "../profile/configsystem.h"
#include "../profile/dialogue.h"
#include "../profile/games/chlcc/dialoguebox.h"

#include "../ui/ui.h"
#include "../data/tipssystem.h"

#include "../hud/waiticondisplay.h"
#include "../hud/autoicondisplay.h"
#include "../hud/skipicondisplay.h"
#include "../hud/nametagdisplay.h"
#include "../hud/tipsnotification.h"

#include "../audio/audiostream.h"
#include "../audio/audiosystem.h"
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

  DialoguePages.reserve(PageCount);
  for (int pageId = 0; pageId < PageCount; pageId++) {
    DialoguePages.emplace_back(pageId).Glyphs.reserve(MaxPageSize);
  }

  for (int i = 0; i < std::ssize(DialoguePages); i++) {
    DialoguePages[i].Clear();
    DialoguePages[i].Id = i;
    DialoguePages[i].CurrentVoiceCharacterId = 0;
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
  TextPage::Clear();

  Name.clear();
  AudioId.reset();
  CurrentStringAddress.reset();

  CurrentLineTop = [this]() {
    switch (this->GetMode()) {
      case DPM_ADV:
        return ADVBounds.Y;
      case DPM_REV:
        return SecondaryREVBounds.Y;
      case DPM_TIPS:
        return TipsBounds.Y;
      default:
      case DPM_NVL:
        return NVLBounds.Y;
    }
  }();
  CurrentLineTopMargin = 0.0f;

  AdvanceMethod = AdvanceMethodType::Skip;
}

void DialoguePage::AddString(Vm::Sc3VmThread* ctx, std::optional<int> voiceId,
                             bool acted, int animId, int charId,
                             bool shouldUpdateCharId) {
  TextFadeAnimation.Reset(AnimationDirection::Out);

  CurrentStringAddress = {ctx->ScriptBufferId, ctx->IpOffset};
  AudioId = voiceId;

  Audio::AudioStream* audioStream = nullptr;
  if (voiceId.has_value()) {
    Io::Stream* stream;
    IoError err = Io::VfsOpen("voice", *voiceId, &stream);

    const bool playAudio = (err == IoError_OK && !GetFlag(SF_MESALLSKIP));
    if (playAudio) audioStream = Audio::AudioStream::Create(stream);
  }

  if (shouldUpdateCharId) {
    CharacterId = charId;
    ScrWork[Id + SW_ANIME0CHANO] = charId;
  }

  AnimationId = std::max(acted ? animId : charId, 31);

  const size_t typeWriterStart = Glyphs.size();

  DialogueTextParserInst.ParseString(*this, ctx);

  RenderName = !Name.empty();

  RectF boundingBox = Glyphs.empty() ? RectF() : Glyphs.begin()->DestRect;
  for (const ProcessedTextGlyph& glyph : Glyphs) {
    boundingBox = RectF::Coalesce(boundingBox, glyph.DestRect);
  }
  Dimensions = glm::vec2(boundingBox.Width, boundingBox.Height);

  // resetting typewriter for a new line and setting new params
  Typewriter.Reset(AnimationDirection::In);
  Typewriter.SetFirstGlyph(typeWriterStart);
  Typewriter.SetGlyphCount(Glyphs.size() - typeWriterStart);
  Typewriter.SetParallelStartGlyphs(DialogueTextParserInst.ParallelStartGlyphs);
  Typewriter.Start(AudioId.has_value());

  if (AudioId.has_value()) {
    CurrentVoiceCharacterId = AnimationId;
    if (audioStream) {
      Audio::Channels[Audio::AC_VOICE0]->Play(
          std::unique_ptr<Audio::AudioStream>(audioStream), false, 0.0f);
    }
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

    for (RubyChunk& chunk : RubyChunks) {
      for (size_t rubyGlyphId = 0; rubyGlyphId < chunk.Length; rubyGlyphId++) {
        chunk.Text[rubyGlyphId].Opacity =
            Typewriter.CalcRubyOpacity(rubyGlyphId, chunk);
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

void DialoguePage::Move(const glm::vec2 relativeOffset) {
  TextPage::Move(relativeOffset);

  for (ProcessedTextGlyph& glyph : Name) {
    glyph.DestRect += relativeOffset;
  }
}

void DialoguePage::Render(const float alpha,
                          const RendererOutlineMode outlineMode) {
  // dialogue text
  if (GetFlag(SF_UIHIDDEN)) return;
  if (FadeAnimation.IsOut()) return;

  glm::vec4 opacityTint(1.0f);
  opacityTint.a = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress) * alpha;

  const NameInfo nameInfo{
      .RenderWindow = RenderName,
      .NameId = ScrWork[SW_MESNAMEID0 + Id] == 0xffff
                    ? std::nullopt
                    : std::optional(ScrWork[SW_MESNAMEID0 + Id]),
      .Name = Name,
  };
  DialogueBoxInst->Render(GetMode(), nameInfo, opacityTint);

  // TODO: Figure out what's up with text box coloring
  glm::vec4 col = glm::vec4(1.0f);  // ScrWorkGetColor(SW_MESWINDOW_COLOR);
  col.a = opacityTint.a;

  const float textFadeOpacity =
      GetFlag(SF_MESALLSKIP) ? 1.0f
                             : opacityTint.a * TextFadeAnimation.Progress;
  Renderer->DrawProcessedText(Glyphs, DialogueFont, textFadeOpacity,
                              outlineMode);
  for (const RubyChunk& chunk : RubyChunks) {
    Renderer->DrawProcessedText(chunk.Text, DialogueFont, textFadeOpacity,
                                outlineMode);
  }

  // Wait icon
  const RectF& lastGlyphDest =
      Glyphs.empty() ? RectF() : Glyphs.back().DestRect;
  glm::vec2 waitIconPos(lastGlyphDest.X + lastGlyphDest.Width, lastGlyphDest.Y);
  WaitIconDisplay::Render(waitIconPos, col, GetMode(), Id);

  AutoIconDisplay::Render(col);
  SkipIconDisplay::Render(col);
}

void DialoguePage::Hide() { FadeAnimation.StartOut(); }

void DialoguePage::Show() { FadeAnimation.StartIn(true); }

void DialoguePage::PushBacklogEntry() {
  if (!CurrentStringAddress.has_value() || GetFlag(SF_REVADDDISABLE) ||
      (ScrWork[SW_MESWIN0TYPE + 10 * Id] & 0b10)) {
    return;
  }

  UI::BacklogMenuPtr->AddMessage(*CurrentStringAddress, AudioId, AnimationId);
}

}  // namespace Impacto
