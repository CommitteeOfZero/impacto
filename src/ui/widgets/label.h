#pragma once

#include "../widget.h"
#include "../../text.h"
#include "../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace Widgets {

class Label : public Widget {
 public:
  Label();
  Label(Sprite const& label, glm::vec2 pos);
  Label(uint8_t* str, glm::vec2 pos, float fontSize,
        RendererOutlineMode outlineMode, int colorIndex = 10);
  Label(ProcessedTextGlyph* str, int textLength, float textWidth,
        float fontSize, RendererOutlineMode outlineMode);
  Label(std::string str, glm::vec2 pos, float fontSize,
        RendererOutlineMode outlineMode, int colorIndex = 10);

  void Update(float dt) override;
  void UpdateInput() override;
  void Render() override;
  void Move(glm::vec2 relativePosition) override;
  void MoveTo(glm::vec2 pos) override;

  void SetSprite(Sprite const& label);
  void SetText(uint8_t* str, float fontSize, RendererOutlineMode outlineMode,
               int colorIndex = 10);
  void SetText(std::string str, float fontSize, RendererOutlineMode outlineMode,
               int colorIndex = 10);
  void SetText(ProcessedTextGlyph* str, int textLength, float textWidth,
               float fontSize, RendererOutlineMode outlineMode,
               int colorIndex = 10);

 protected:
  bool IsText;
  Sprite LabelSprite;
  float FontSize;
  ProcessedTextGlyph Text[255];
  int TextLength = 0;
  float TextWidth = 0.0f;
  RendererOutlineMode OutlineMode = RO_None;
  int ColorIndex;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto