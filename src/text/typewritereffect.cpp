#include "typewritereffect.h"

#include "../profile/configsystem.h"
#include "../profile/dialogue.h"

#include "../audio/audiochannel.h"
#include "../audio/audiosystem.h"

namespace Impacto {
using namespace Profile::ConfigSystem;
using namespace Profile::Dialogue;

static float CalcLinearProgress(float progress, float startProgress,
                                float endProgress) {
  if (endProgress <= startProgress)
    return progress >= endProgress ? 1.0f : 0.0f;
  return std::clamp((progress - startProgress) / (endProgress - startProgress),
                    0.0f, 1.0f);
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

static std::pair<float, float> GetWritingProgresses(const size_t glyphNo,
                                                    const size_t glyphCount,
                                                    const float totalProgress) {
  if (glyphCount == 0 || totalProgress <= 0.0f) return {0.0f, 1.0f};

  const float glyphFadeProgress = std::min(TextFadeInDuration, totalProgress);

  float startTime = 0.0f;
  if (glyphCount > 1 && totalProgress > glyphFadeProgress) {
    const float glyphStartInterval =
        (totalProgress - glyphFadeProgress) / (glyphCount - 1);
    startTime = glyphStartInterval * glyphNo;
  }

  const float endTime = std::min(startTime + glyphFadeProgress, totalProgress);

  return {startTime / totalProgress, endTime / totalProgress};
}

std::pair<float, float> TypewriterEffect::GetGlyphWritingProgresses(
    const size_t glyph) {
  const ParallelBlock block = GetParallelBlock(glyph);

  return GetWritingProgresses(glyph - block.Start, block.Size, 1.0f);
}

float TypewriterEffect::CalcOpacity(size_t glyph) {
  if (glyph < FirstGlyph) return 1.0f;
  if (glyph >= FirstGlyph + GlyphCount) return 0.0f;

  const auto [startProgress, endProgress] = GetGlyphWritingProgresses(glyph);

  if (!IsCancelled) {
    return CalcLinearProgress(Progress, startProgress, endProgress);
  }

  if (ProgressOnCancel >= endProgress) return 1.0f;

  const float cancelProgress =
      (Progress - ProgressOnCancel) / (1.0f - ProgressOnCancel);

  if (ProgressOnCancel <= startProgress) return cancelProgress;

  const float glyphProgressBeforeCancellation =
      CalcLinearProgress(ProgressOnCancel, startProgress, endProgress);
  return glyphProgressBeforeCancellation +
         cancelProgress * (1.0f - glyphProgressBeforeCancellation);
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
  const auto [glyphStartProgressInBase, glyphEndProgressInBase] =
      GetWritingProgresses(rubyGlyphId, chunk.Length, baseProgressLength);

  // Convert back to progress-space
  const float startProgress =
      baseStartProgress + glyphStartProgressInBase * baseProgressLength;
  const float endProgress =
      baseStartProgress + glyphEndProgressInBase * baseProgressLength;

  if (IsCancelled) {
    if (ProgressOnCancel >= endProgress) return 1.0f;

    const float cancelProgress =
        (Progress - ProgressOnCancel) / (1.0f - ProgressOnCancel);
    if (ProgressOnCancel <= startProgress) return cancelProgress;

    const float glyphProgressBeforeCancellation =
        CalcLinearProgress(ProgressOnCancel, startProgress, endProgress);
    return glyphProgressBeforeCancellation +
           cancelProgress * (1.0f - glyphProgressBeforeCancellation);
  }

  return CalcLinearProgress(Progress, startProgress, endProgress);
}
}  // namespace Impacto
