#pragma once

#include "../widget.h"
#include "../../text.h"

namespace Impacto {
namespace UI {
namespace Widgets {

class Label : public Widget {
 public:
  Label(uint8_t* str, glm::vec2 pos, int fontSize, bool outline);
  Label(ProcessedTextGlyph* str, int textLength, float textWidth, int fontSize,
        bool outline);

  void Update(float dt) override;
  void UpdateInput();
  void Render();

  void SetText(uint8_t* str, int fontSize, bool outline);
  void SetText(ProcessedTextGlyph* str, int textLength, float textWidth,
               int fontSize, bool outline);

 private:
  int FontSize;
  ProcessedTextGlyph Text[255];
  int TextLength;
  float TextWidth = 0.0f;
  bool Outline = false;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto