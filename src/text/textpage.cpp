#include "textpage.h"

#include "../profile/dialogue.h"

#include <numeric>

namespace Impacto {
using namespace Impacto::Profile::Dialogue;

void TextPage::Clear() {
  Glyphs.clear();
  RubyChunks.clear();

  Bounds = RectF{};

  CurrentLineTopMargin = 0.0f;
}

void TextPage::Move(const glm::vec2 relativePos) {
  for (ProcessedTextGlyph& glyph : Glyphs) {
    glyph.DestRect += relativePos;
  }
  for (RubyChunk& chunk : RubyChunks) {
    for (ProcessedTextGlyph& glyph : chunk.Text) {
      glyph.DestRect += relativePos;
    }
  }
}

void TextPage::MoveTo(const glm::vec2 pos) {
  if (Glyphs.empty()) return;

  const glm::vec2 relativePos = pos - Glyphs[0].DestRect.GetPos();
  Move(relativePos);
}

void TextPage::Render(const float alpha,
                      const RendererOutlineMode outlineMode) {
  Renderer->DrawProcessedText(Glyphs, DialogueFont, alpha, outlineMode);
  for (RubyChunk& chunk : RubyChunks) {
    Renderer->DrawProcessedText(chunk.Text, DialogueFont, alpha, outlineMode);
  }
}

RectF TextPage::SetBounds() {
  if (Glyphs.empty()) return Bounds = RectF{};

  const auto coalesce = [](const RectF bounds,
                           const ProcessedTextGlyph& glyph) {
    return RectF::Coalesce(bounds, glyph.DestRect);
  };

  Bounds = Glyphs.front().DestRect;
  Bounds = std::accumulate(Glyphs.begin() + 1, Glyphs.end(), Bounds, coalesce);
  for (const RubyChunk& chunk : RubyChunks) {
    Bounds =
        std::accumulate(chunk.Text.begin(), chunk.Text.end(), Bounds, coalesce);
  }

  return Bounds;
}

}  // namespace Impacto
