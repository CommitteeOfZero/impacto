#include "label.h"
#include "../../vm/thread.h"
#include "../../profile/dialogue.h"

namespace Impacto {
namespace UI {
namespace Widgets {

Label::Label() {}

Label::Label(Sprite const& label, glm::vec2 pos) {
  IsText = false;
  LabelSprite = label;
  Bounds =
      RectF(pos.x, pos.y, LabelSprite.Bounds.Width, LabelSprite.Bounds.Height);
}

Label::Label(uint8_t* str, glm::vec2 pos, int fontSize,
             RendererOutlineMode outlineMode, int colorIndex) {
  FontSize = fontSize;
  Bounds = RectF(pos.x, pos.y, TextLength, FontSize);
  SetText(str, fontSize, outlineMode, colorIndex);
}

Label::Label(ProcessedTextGlyph* str, int textLength, float textWidth,
             int fontSize, RendererOutlineMode outlineMode) {
  FontSize = fontSize;
  SetText(str, textLength, textWidth, fontSize, outlineMode);
}

Label::Label(std::string str, glm::vec2 pos, int fontSize,
             RendererOutlineMode outlineMode, int colorIndex) {
  FontSize = fontSize;
  Bounds = RectF(pos.x, pos.y, TextLength, FontSize);
  SetText(str, fontSize, outlineMode, colorIndex);
}

void Label::UpdateInput() {}

void Label::Update(float dt) { Widget::Update(dt); }

void Label::Render() {
  if (IsText) {
    Renderer->DrawProcessedText(Text, TextLength,
                                Profile::Dialogue::DialogueFont, Tint.a,
                                OutlineMode, true);
  } else {
    Renderer->DrawSprite(LabelSprite, Bounds, Tint);
  }
}

void Label::Move(glm::vec2 relativePosition) {
  for (int i = 0; i < TextLength; i++) {
    Text[i].DestRect.X += relativePosition.x;
    Text[i].DestRect.Y += relativePosition.y;
  }
  Widget::Move(relativePosition);
}

void Label::MoveTo(glm::vec2 pos) {
  auto relativePosition = pos - glm::vec2(Bounds.X, Bounds.Y);
  Move(relativePosition);
}

void Label::SetSprite(Sprite const& label) {
  IsText = false;
  LabelSprite = label;
  Bounds = RectF(Bounds.X, Bounds.Y, LabelSprite.Bounds.Width,
                 LabelSprite.Bounds.Height);
}

void Label::SetText(uint8_t* str, int fontSize, RendererOutlineMode outlineMode,
                    int colorIndex) {
  IsText = true;
  ColorIndex = colorIndex;
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  TextLength = TextLayoutPlainLine(
      &dummy, 255, Text, Profile::Dialogue::DialogueFont, fontSize,
      Profile::Dialogue::ColorTable[ColorIndex], 1.0f,
      glm::vec2(Bounds.X, Bounds.Y), TextAlignment::Left);
  OutlineMode = outlineMode;
  TextWidth = 0.0f;
  for (int i = 0; i < TextLength; i++) {
    TextWidth += Text[i].DestRect.Width;
  }
  Bounds = RectF(Text[0].DestRect.X, Text[0].DestRect.Y, TextWidth, fontSize);
}

void Label::SetText(ProcessedTextGlyph* str, int textLength, float textWidth,
                    int fontSize, RendererOutlineMode outlineMode,
                    int colorIndex) {
  IsText = true;
  ColorIndex = colorIndex;
  TextLength = textLength;
  TextWidth = textWidth;
  OutlineMode = outlineMode;
  memcpy(Text, str, TextLength * sizeof(ProcessedTextGlyph));
  Bounds = RectF(Text[0].DestRect.X, Text[0].DestRect.Y, TextWidth, fontSize);
}

void Label::SetText(std::string str, int fontSize,
                    RendererOutlineMode outlineMode, int colorIndex) {
  IsText = true;
  ColorIndex = colorIndex;
  TextLength = TextLayoutPlainString(
      str, Text, Profile::Dialogue::DialogueFont, fontSize,
      Profile::Dialogue::ColorTable[ColorIndex], 1.0f,
      glm::vec2(Bounds.X, Bounds.Y), TextAlignment::Left);
  OutlineMode = outlineMode;
  TextWidth = 0.0f;
  for (int i = 0; i < TextLength; i++) {
    TextWidth += Text[i].DestRect.Width;
  }
  Bounds = RectF(Text[0].DestRect.X, Text[0].DestRect.Y, TextWidth, fontSize);
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto