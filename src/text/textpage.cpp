#include "textpage.h"

#include "../profile/dialogue.h"

namespace Impacto {
using namespace Impacto::Profile::Dialogue;

void TextPage::Clear() {
  Glyphs.clear();
  RubyChunks.clear();

  CurrentLineTopMargin = 0.0f;
}

void TextPage::Move(const glm::vec2 relativePos) {
  for (ProcessedTextGlyph& glyph : Glyphs) {
    glyph.DestRect += relativePos;
  }
  for (RubyChunk& chunk : RubyChunks) {
    for (ProcessedTextGlyph& glyph :
         std::span(chunk.Text.begin(), chunk.Length)) {
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

}  // namespace Impacto
