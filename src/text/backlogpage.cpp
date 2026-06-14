#include "backlogpage.h"

#include "textparser.h"

#include <numeric>

namespace Impacto {

void BacklogPage::Clear() {
  TextPage::Clear();

  Name.clear();
}

void BacklogPage::AddString(Vm::Sc3VmThread* const ctx) {
  Clear();
  BacklogTextParserInst.ParseString(*this, ctx);

  SetBounds();
}

void BacklogPage::Move(const glm::vec2 relativePos) {
  TextPage::Move(relativePos);

  for (ProcessedTextGlyph& glyph : Name) {
    glyph.DestRect += relativePos;
  }
}

void BacklogPage::Render(const float alpha,
                         const RendererOutlineMode outlineMode) {
  TextPage::Render(alpha, outlineMode);
  Renderer->DrawProcessedText(Name, DialogueFont, alpha, outlineMode);
}

RectF BacklogPage::SetBounds() {
  if (Glyphs.empty()) return Bounds = RectF{};
  TextPage::SetBounds();

  Bounds =
      std::accumulate(Name.begin(), Name.end(), Bounds,
                      [](const RectF bounds, const ProcessedTextGlyph& glyph) {
                        return RectF::Coalesce(bounds, glyph.DestRect);
                      });

  return Bounds;
}

}  // namespace Impacto
