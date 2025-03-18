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

Label::Label(uint8_t* str, glm::vec2 pos, float fontSize,
             RendererOutlineMode outlineMode, int colorIndex) {
  FontSize = fontSize;
  Bounds = RectF(pos.x, pos.y, 0, FontSize);
  SetText(str, fontSize, outlineMode, colorIndex);
}

Label::Label(std::vector<ProcessedTextGlyph> str, float textWidth,
             float fontSize, RendererOutlineMode outlineMode) {
  FontSize = fontSize;
  SetText(std::move(str), textWidth, fontSize, outlineMode);
}

Label::Label(std::span<ProcessedTextGlyph> str, float textWidth, float fontSize,
             RendererOutlineMode outlineMode) {
  FontSize = fontSize;
  SetText(std::move(str), textWidth, fontSize, outlineMode);
}

Label::Label(std::string_view str, glm::vec2 pos, float fontSize,
             RendererOutlineMode outlineMode, int colorIndex) {
  FontSize = fontSize;
  Bounds = RectF(pos.x, pos.y, 0, FontSize);
  SetText(str, fontSize, outlineMode, colorIndex);
}

Label::Label(uint8_t* str, glm::vec2 pos, float fontSize,
             RendererOutlineMode outlineMode, DialogueColorPair colorPair) {
  FontSize = fontSize;
  Bounds = RectF(pos.x, pos.y, 0, FontSize);
  SetText(str, fontSize, outlineMode, colorPair);
}

Label::Label(std::string_view str, glm::vec2 pos, float fontSize,
             RendererOutlineMode outlineMode, DialogueColorPair colorPair) {
  FontSize = fontSize;
  Bounds = RectF(pos.x, pos.y, 0, FontSize);
  SetText(str, fontSize, outlineMode, colorPair);
}

void Label::UpdateInput() {}

void Label::Update(float dt) { Widget::Update(dt); }

void Label::Render() {
  if (IsText) {
    if (OutlineAlphaEnabled) {
      Renderer->DrawProcessedText(Text, Profile::Dialogue::DialogueFont, Tint.a,
                                  OutlineAlpha, OutlineMode, true);
    } else {
      Renderer->DrawProcessedText(Text, Profile::Dialogue::DialogueFont, Tint.a,
                                  OutlineMode, true);
    }
  } else {
    Renderer->DrawSprite(LabelSprite, Bounds, Tint);
  }
}

void Label::Move(glm::vec2 relativePosition) {
  for (int i = 0; i < Text.size(); i++) {
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

void Label::SetText(std::vector<ProcessedTextGlyph> str, float textWidth,
                    float fontSize, RendererOutlineMode outlineMode) {
  IsText = true;
  OutlineMode = outlineMode;
  Text = std::move(str);
  Bounds = RectF(Text[0].DestRect.X, Text[0].DestRect.Y, TextWidth, fontSize);
}

void Label::SetText(std::span<ProcessedTextGlyph> str, float textWidth,
                    float fontSize, RendererOutlineMode outlineMode) {
  IsText = true;
  OutlineMode = outlineMode;
  Text = std::vector<ProcessedTextGlyph>(str.begin(), str.end());
  Bounds = RectF(Text[0].DestRect.X, Text[0].DestRect.Y, TextWidth, fontSize);
}

void Label::SetText(std::string_view str, float fontSize,
                    RendererOutlineMode outlineMode, int colorIndex) {
  SetText(str, fontSize, outlineMode,
          Profile::Dialogue::ColorTable[colorIndex]);
}

void Label::SetText(uint8_t* str, float fontSize,
                    RendererOutlineMode outlineMode, int colorIndex) {
  SetText(str, fontSize, outlineMode,
          Profile::Dialogue::ColorTable[colorIndex]);
}

void Label::SetText(uint8_t* str, float fontSize,
                    RendererOutlineMode outlineMode,
                    DialogueColorPair colorPair) {
  IsText = true;
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  FontSize = fontSize;
  Text = TextLayoutPlainLine(
      &dummy, 255, Profile::Dialogue::DialogueFont, fontSize, colorPair, 1.0f,
      glm::vec2(Bounds.X, Bounds.Y), TextAlignment::Left);
  OutlineMode = outlineMode;
  TextWidth = 0.0f;
  for (int i = 0; i < Text.size(); i++) {
    TextWidth += Text[i].DestRect.Width;
  }
  Bounds = RectF(Text[0].DestRect.X, Text[0].DestRect.Y, TextWidth, fontSize);
}

void Label::SetText(std::string_view str, float fontSize,
                    RendererOutlineMode outlineMode,
                    DialogueColorPair colorPair) {
  IsText = true;
  Text = TextLayoutPlainString(str, Profile::Dialogue::DialogueFont, fontSize,
                               colorPair, 1.0f, glm::vec2(Bounds.X, Bounds.Y),
                               TextAlignment::Left);
  OutlineMode = outlineMode;
  TextWidth = 0.0f;
  for (int i = 0; i < Text.size(); i++) {
    TextWidth += Text[i].DestRect.Width;
  }
  if (Text.size() > 0) {
    Bounds = RectF(Text[0].DestRect.X, Text[0].DestRect.Y, TextWidth, fontSize);
  }
}
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto