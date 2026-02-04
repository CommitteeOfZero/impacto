#include "typewritereffect.h"

#include "../profile/configsystem.h"
#include "../profile/dialogue.h"

#include "../audio/audiochannel.h"
#include "../audio/audiosystem.h"

namespace Impacto {
using namespace Profile::ConfigSystem;
using namespace Profile::Dialogue;

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
}  // namespace Impacto