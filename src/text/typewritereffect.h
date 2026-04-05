#pragma once

#include <set>

#include "text.h"
#include "../animation.h"

namespace Impacto {

struct TypewriterEffect : public Animation {
 public:
  void Start(bool voiced);
  void Update(float dt);

  float CalcOpacity(size_t glyph);
  float CalcRubyOpacity(size_t rubyGlyphId, const RubyChunk& chunk);

  void SetGlyphCount(size_t glyphCount) { GlyphCount = glyphCount; }
  size_t GetGlyphCount() const { return GlyphCount; }

  void SetParallelStartGlyphs(const std::set<size_t>& parallelStartGlyphs) {
    ParallelStartGlyphs = parallelStartGlyphs;
  }
  void SetFirstGlyph(size_t firstGlyph) { FirstGlyph = firstGlyph; }

  bool CancelRequested = false;
  bool IsCancelled = false;

 private:
  size_t FirstGlyph = 0;
  size_t GlyphCount = 0;

  std::set<size_t> ParallelStartGlyphs;
  float ProgressOnCancel;

  bool Voiced = false;

  struct ParallelBlock {
    size_t Start;
    size_t Size;
  };
  ParallelBlock GetParallelBlock(size_t glyph);

  float GetTypewriterDurationSeconds() const;
  float GetSingleGlyphFadeDurationSeconds(float totalDurationSeconds) const;
  // {startProgress, endProgress} in [0, 1]
  std::pair<float, float> GetWritingProgresses(
      size_t glyphNo, size_t glyphCount, float totalDurationSeconds) const;
  // {startProgress, endProgress}
  std::pair<float, float> GetGlyphWritingProgresses(size_t glyph);
};
}  // namespace Impacto
