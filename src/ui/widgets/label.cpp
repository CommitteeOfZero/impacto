#include "label.h"
#include "../../vm/thread.h"
#include "../../profile/dialogue.h"

#include <numeric>

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

Label::Label(Vm::BufferOffsetContext scrCtx, glm::vec2 pos, float fontSize,
             RendererOutlineMode outlineMode, int colorIndex)
    : Label(scrCtx, pos, fontSize, outlineMode,
            Profile::Dialogue::ColorTable[colorIndex]) {}

Label::Label(std::string_view str, glm::vec2 pos, float fontSize,
             RendererOutlineMode outlineMode, int colorIndex)
    : Label(str, pos, fontSize, outlineMode,
            Profile::Dialogue::ColorTable[colorIndex]) {}

Label::Label(Vm::Sc3Stream& stream, glm::vec2 pos, float fontSize,
             RendererOutlineMode outlineMode, int colorIndex)
    : Label(stream, pos, fontSize, outlineMode,
            Profile::Dialogue::ColorTable[colorIndex]) {}

Label::Label(Vm::BufferOffsetContext scrCtx, glm::vec2 pos, float fontSize,
             RendererOutlineMode outlineMode, DialogueColorPair colorPair) {
  FontSize = fontSize;
  Bounds = RectF(pos.x, pos.y, 0, FontSize);
  SetText(scrCtx, fontSize, outlineMode, colorPair);
}

Label::Label(Vm::Sc3Stream& stream, glm::vec2 pos, float fontSize,
             RendererOutlineMode outlineMode, DialogueColorPair colorPair) {
  FontSize = fontSize;
  Bounds = RectF(pos.x, pos.y, 0, FontSize);
  SetText(stream, fontSize, outlineMode, colorPair);
}

Label::Label(std::string_view str, glm::vec2 pos, float fontSize,
             RendererOutlineMode outlineMode, DialogueColorPair colorPair) {
  FontSize = fontSize;
  Bounds = RectF(pos.x, pos.y, 0, FontSize);
  SetText(str, fontSize, outlineMode, colorPair);
}

void Label::UpdateInput(float dt) {}

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
  for (ProcessedTextGlyph& glyph : Text) {
    glyph.DestRect += relativePosition;
  }
  Widget::Move(relativePosition);
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

void Label::SetText(std::span<ProcessedTextGlyph> str,
                    RendererOutlineMode outlineMode) {
  IsText = true;
  OutlineMode = outlineMode;
  Text = std::vector<ProcessedTextGlyph>(str.begin(), str.end());

  Bounds = std::reduce(str.begin() + 1, str.end(), str[0],
                       [](auto lhs, const auto& rhs) {
                         lhs.DestRect =
                             RectF::Coalesce(lhs.DestRect, rhs.DestRect);
                         return lhs;
                       })
               .DestRect;
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

void Label::SetText(Vm::BufferOffsetContext scrCtx, float fontSize,
                    RendererOutlineMode outlineMode, int colorIndex) {
  SetText(scrCtx, fontSize, outlineMode,
          Profile::Dialogue::ColorTable[colorIndex]);
}
void Label::SetText(Vm::Sc3Stream& stream, float fontSize,
                    RendererOutlineMode outlineMode, int colorIndex) {
  SetText(stream, fontSize, outlineMode,
          Profile::Dialogue::ColorTable[colorIndex]);
}

void Label::SetText(Vm::Sc3Stream& stream, float fontSize,
                    RendererOutlineMode outlineMode,
                    DialogueColorPair colorPair) {
  IsText = true;
  FontSize = fontSize;
  Text = TextLayoutPlainLine(
      stream, 255, Profile::Dialogue::DialogueFont, fontSize, colorPair, 1.0f,
      glm::vec2(Bounds.X, Bounds.Y), TextAlignment::Left);
  OutlineMode = outlineMode;
  TextWidth = 0.0f;
  for (const ProcessedTextGlyph& glyph : Text) {
    TextWidth += glyph.DestRect.Width;
  }
  Bounds = RectF(Text[0].DestRect.X, Text[0].DestRect.Y, TextWidth, fontSize);
}

void Label::SetText(Vm::BufferOffsetContext scrCtx, float fontSize,
                    RendererOutlineMode outlineMode,
                    DialogueColorPair colorPair) {
  IsText = true;
  Impacto::Vm::Sc3VmThread dummy;
  dummy.IpOffset = scrCtx.IpOffset;
  dummy.ScriptBufferId = scrCtx.ScriptBufferId;
  FontSize = fontSize;
  Text = TextLayoutPlainLine(
      &dummy, 255, Profile::Dialogue::DialogueFont, fontSize, colorPair, 1.0f,
      glm::vec2(Bounds.X, Bounds.Y), TextAlignment::Left);
  OutlineMode = outlineMode;
  TextWidth = 0.0f;
  for (const ProcessedTextGlyph& glyph : Text) {
    TextWidth += glyph.DestRect.Width;
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
  for (const ProcessedTextGlyph& glyph : Text) {
    TextWidth += glyph.DestRect.Width;
  }
  if (Text.size() > 0) {
    Bounds = RectF(Text[0].DestRect.X, Text[0].DestRect.Y, TextWidth, fontSize);
  }
}
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto