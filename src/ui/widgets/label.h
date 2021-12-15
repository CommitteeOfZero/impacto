#pragma once

#include "../widget.h"
#include "../../text.h"

namespace Impacto {
namespace UI {
namespace Widgets {

class Label : public Widget {
 public:
  Label();
  Label(Sprite const& label, glm::vec2 pos);
  Label(uint8_t* str, glm::vec2 pos, int fontSize, bool outline,
        int colorIndex = 10);
  Label(ProcessedTextGlyph* str, int textLength, float textWidth, int fontSize,
        bool outline);
  Label(std::string str, glm::vec2 pos, int fontSize, bool outline,
        int colorIndex = 10);

  void Update(float dt) override;
  void UpdateInput();
  void Render();
  void Move(glm::vec2 relativePosition) override;
  void MoveTo(glm::vec2 pos) override;

  void SetSprite(Sprite const& label);
  void SetText(uint8_t* str, int fontSize, bool outline, int colorIndex = 10);
  void SetText(std::string str, int fontSize, bool outline,
               int colorIndex = 10);
  void SetText(ProcessedTextGlyph* str, int textLength, float textWidth,
               int fontSize, bool outline, int colorIndex = 10);

 protected:
  bool IsText;
  Sprite LabelSprite;
  int FontSize;
  ProcessedTextGlyph Text[255];
  int TextLength;
  float TextWidth = 0.0f;
  bool Outline = false;
  int ColorIndex;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto