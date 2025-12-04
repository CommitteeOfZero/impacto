#pragma once

#include "../button.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

class SystemMessageButton : public Button {
 public:
  SystemMessageButton(int id, Sprite const& norm, Sprite const& focused,
                      Sprite const& highlightLeft,
                      Sprite const& highlightMiddle,
                      Sprite const& highlightRight, glm::vec2 pos,
                      RectF hoverBounds = RectF{});

  virtual void Render() override;

  void SetText(std::vector<ProcessedTextGlyph> text, float textWidth,
               float fontSize, RendererOutlineMode outlineMode) override;

 protected:
  Sprite LeftHighlightSprite;
  Sprite RightHighlightSprite;
  glm::vec2 LeftHighlightPos;
  glm::vec2 RightHighlightPos;
};

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto