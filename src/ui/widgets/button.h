#pragma once

#include <functional>

#include "../widget.h"
#include "../../spritesheet.h"
#include "../../text.h"

namespace Impacto {
namespace UI {
namespace Widgets {

class Button : public Widget {
 public:
  Button() {}
  Button(int id, Sprite const& norm, Sprite const& focused,
         Sprite const& highlight, glm::vec2 pos);

  void UpdateInput();
  virtual void Render();

  void SetText(uint8_t* str, int fontSize, bool outline, int colorIndex = 10);
  void SetText(ProcessedTextGlyph* str, int textLength, float textWidth,
               int fontSize, bool outline);

  int Id;
  Sprite NormalSprite;
  Sprite FocusedSprite;
  Sprite HighlightSprite;
  Sprite DisabledSprite;

  std::function<void(Button*)> OnClickHandler;

  glm::vec2 HighlightOffset = glm::vec2(0.0f, 3.0f);

 protected:
  bool HasText = false;

  ProcessedTextGlyph Text[255];
  int TextLength = 0;
  float TextWidth = 0.0f;
  bool Outline = false;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto