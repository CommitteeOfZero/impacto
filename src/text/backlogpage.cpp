#include "backlogpage.h"

#include "textparser.h"

namespace Impacto {

void BacklogPage::Clear() {
  TextPage::Clear();

  Name.clear();
}

void BacklogPage::AddString(Vm::Sc3VmThread* const ctx) {
  Clear();
  BacklogTextParserInst.ParseString(*this, ctx);
}

void BacklogPage::Move(const glm::vec2 relativePos) {
  TextPage::Move(relativePos);

  for (ProcessedTextGlyph& glyph : Name) {
    glyph.DestRect += relativePos;
  }
}

void BacklogPage::Render(const float alpha,
                         const RendererOutlineMode outlineMode) {
  Renderer->DrawProcessedText(Glyphs, DialogueFont, alpha, outlineMode);
  for (const RubyChunk& chunk : RubyChunks) {
    Renderer->DrawProcessedText(chunk.Text, DialogueFont, alpha, outlineMode);
  }
  Renderer->DrawProcessedText(Name, DialogueFont, alpha, outlineMode);
}

}  // namespace Impacto
