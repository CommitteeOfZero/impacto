#pragma once

#include "textpage.h"

#include "../audio/audiostream.h"

namespace Impacto {

struct BacklogPage : public TextPage {
 public:
  std::vector<ProcessedTextGlyph> Name;

  void Clear() override;
  void AddString(Vm::Sc3VmThread* ctx);

  void Move(glm::vec2 relativePos) override;

  void Render(float alpha, RendererOutlineMode outlineMode =
                               RendererOutlineMode::Full) override;
};

}  // namespace Impacto
