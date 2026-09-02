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
    glyph.Move(relativePos);
  }
  for (RubyChunk& chunk : RubyChunks) {
    for (ProcessedTextGlyph& glyph : chunk.Text) {
      glyph.Move(relativePos);
    }
  }
}

void TextPage::MoveTo(const glm::vec2 pos) {
  if (Glyphs.empty()) return;

  const glm::vec2 relativePos = pos - Glyphs.front().Position;
  Move(relativePos);
}

void TextPage::Render(const float alpha,
                      const RendererOutlineMode outlineMode) {
  DialogueFont->DrawProcessedText(Glyphs, alpha, outlineMode);
  for (RubyChunk& chunk : RubyChunks) {
    DialogueFont->DrawProcessedText(chunk.Text, alpha, outlineMode);
  }
}

RectF TextPage::SetBounds() {
  if (Glyphs.empty()) return Bounds = RectF{};

  Bounds = GetTextBounds(Glyphs);
  for (const RubyChunk& chunk : RubyChunks) {
    Bounds = RectF::Coalesce(Bounds, GetTextBounds(chunk.Text));
  }

  return Bounds;
}

}  // namespace Impacto
