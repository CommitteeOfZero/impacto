#include "typewritereffect.h"

#include "../profile/configsystem.h"
#include "../profile/dialogue.h"

#include "../audio/audiochannel.h"
#include "../audio/audiosystem.h"

namespace Impacto {
using namespace Profile::ConfigSystem;
using namespace Profile::Dialogue;

void TypewriterEffect::Start(const bool voiced) {
  Voiced = voiced;
  IsCancelled = false;
  StartIn(true);

  if (GlyphCount == 0) Finish();
}

void TypewriterEffect::Update(float dt) {
  if (State == AnimationState::Stopped) return;

  if (CancelRequested && !IsCancelled) {
    IsCancelled = true;
    CancelRequested = false;
    Finish();
  }

  if (Direction == AnimationDirection::In && !IsCancelled) {
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

void TypewriterEffect::UpdateOpacity(const std::span<ProcessedTextGlyph> glyphs,
                                     const std::span<RubyChunk> rubyChunks,
                                     const float dt) const {
  if (GetFlag(Profile::ScriptVars::SF_MESALLSKIP)) {
    // Show all glyphs at full opacity
    for (auto& glyph : glyphs) {
      glyph.Opacity = 1.0f;
    }

    for (auto& chunk : rubyChunks) {
      for (auto& glyph : chunk.Text) {
        glyph.Opacity = 1.0f;
      }
    }

    return;
  }

  if (Direction == AnimationDirection::Out) {
    // Gradually fade out all glyphs
    for (auto& glyph : glyphs) {
      glyph.Opacity = std::min(glyph.Opacity, Progress);
    }

    for (auto& chunk : rubyChunks) {
      for (auto& glyph : chunk.Text) {
        glyph.Opacity = std::min(glyph.Opacity, Progress);
      }
    }

    return;
  }

  // Fading in:
  // Place each parallel block's glyphs' fading start times equidistantly from
  // each other, such that the first glyph starts fading at progress 0.0, and
  // the last glyph starts fading at progress 1.0
  //
  // If the typewriter is cancelled, or the text speed is set to instant
  // (without it being a voiced line and SyncVoice being enabled), then all
  // glyphs should start appearing immediately
  const bool instantTextSpeed = Profile::ConfigSystem::TextSpeed >=
                                Profile::ConfigSystem::TextSpeedBounds.y;
  const bool syncVoice = Voiced && Profile::ConfigSystem::SyncVoice;
  const bool showInstant = IsCancelled || (instantTextSpeed && !syncVoice);

  // Make it so a glyph fading from 0.0 -> 1.0 opacity takes
  // TextFadeInDuration seconds
  const float deltaOpacity = dt / TextFadeInDuration;

  for (size_t glyphIdx = 0; glyphIdx < glyphs.size(); glyphIdx++) {
    float& opacity = glyphs[glyphIdx].Opacity;

    if (glyphIdx < FirstGlyph) {
      // From a prior line of the same NVL block; keep at full opacity
      opacity = 1.0f;
      continue;
    }

    if (showInstant || Progress >= GetGlyphStartProgress(glyphIdx)) {
      opacity = std::min(opacity + deltaOpacity, 1.0f);
      continue;
    }

    opacity = 0.0f;
  }

  // Place all ruby glyphs' fading start times equidistantly from each other,
  // such that the first glyph starts fading simultaneously with the base's
  // first glyph, and the last glyph starts fading simultaneously with the
  // base's last glyph
  for (auto& chunk : rubyChunks) {
    const float baseStartProgress =
        GetGlyphStartProgress(chunk.FirstBaseCharacter);
    const float baseEndProgress =
        GetGlyphStartProgress(chunk.FirstBaseCharacter + chunk.BaseLength - 1);

    for (size_t glyphIdx = 0; glyphIdx < chunk.Text.size(); glyphIdx++) {
      float& opacity = chunk.Text[glyphIdx].Opacity;

      const float ratioInChunk =
          chunk.Text.size() == 1
              ? 0.0f
              : glyphIdx / static_cast<float>(chunk.Text.size() - 1);
      const float startProgress =
          glm::mix(baseStartProgress, baseEndProgress, ratioInChunk);
      if (showInstant || Progress >= startProgress) {
        opacity = std::min(opacity + deltaOpacity, 1.0f);
        continue;
      }

      opacity = 0.0f;
    }
  }
}

TypewriterEffect::ParallelBlock TypewriterEffect::GetParallelBlock(
    const size_t glyph) const {
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

float TypewriterEffect::GetGlyphStartProgress(const size_t glyphIdx) const {
  if (glyphIdx <= FirstGlyph) return 0.0f;
  if (glyphIdx >= FirstGlyph + GlyphCount) {
    return std::numeric_limits<float>::max();
  }

  const ParallelBlock block = GetParallelBlock(glyphIdx);
  const size_t parallelBlockGlyphNo = glyphIdx - block.Start;
  return block.Size == 1
             ? 0.0f
             : parallelBlockGlyphNo / static_cast<float>(block.Size - 1);
}

}  // namespace Impacto
