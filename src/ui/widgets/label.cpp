#include "label.h"
#include "../../vm/thread.h"
#include "../../profile/dialogue.h"
#include "../../renderer2d.h"

namespace Impacto {
namespace UI {
namespace Widgets {

Label::Label(uint8_t* str, glm::vec2 pos, int fontSize, bool outline) {
  FontSize = fontSize;
  Bounds = RectF(pos.x, pos.y, TextLength, FontSize);
  SetText(str, fontSize, outline);
}

Label::Label(ProcessedTextGlyph* str, int textLength, float textWidth,
             int fontSize, bool outline) {
  FontSize = fontSize;
  SetText(str, textLength, textWidth, fontSize, outline);
}

void Label::UpdateInput() {}

void Label::Update(float dt) { Widget::Update(dt); }

void Label::Render() {
  Renderer2D::DrawProcessedText(Text, TextLength,
                                Profile::Dialogue::DialogueFont, Opacity,
                                Outline, true);
}

void Label::SetText(uint8_t* str, int fontSize, bool outline) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  TextLength =
      TextLayoutPlainLine(&dummy, 255, Text, Profile::Dialogue::DialogueFont,
                          fontSize, Profile::Dialogue::ColorTable[10], 1.0f,
                          glm::vec2(Bounds.X, Bounds.Y), TextAlignment::Left);
  Outline = outline;
  for (int i = 0; i < TextLength; i++) {
    TextWidth += Text[i].DestRect.Width;
  }
  Bounds = RectF(Text[0].DestRect.X, Text[0].DestRect.Y, TextWidth, fontSize);
}

void Label::SetText(ProcessedTextGlyph* str, int textLength, float textWidth,
                    int fontSize, bool outline) {
  TextLength = textLength;
  TextWidth = textWidth;
  Outline = outline;
  memcpy(Text, str, TextLength * sizeof(ProcessedTextGlyph));
  Bounds = RectF(Text[0].DestRect.X, Text[0].DestRect.Y, TextWidth, fontSize);
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto