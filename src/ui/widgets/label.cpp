#include "label.h"
#include "../../vm/thread.h"

#include <numeric>

namespace Impacto {
namespace UI {
namespace Widgets {

Label::Label() {}

Label::Label(Sprite const& label, glm::vec2 pos)
    : IsText(false), LabelSprite(label) {
  Bounds = RectF(pos.x, pos.y, LabelSprite.ScaledWidth(),
                 LabelSprite.ScaledHeight());
}

void Label::Render() {
  if (IsText) {
    if (OutlineAlphaEnabled) {
      Profile::Dialogue::DialogueFont->DrawProcessedText(
          Text, Tint.a, OutlineAlpha, OutlineMode);
    } else {
      Profile::Dialogue::DialogueFont->DrawProcessedText(Text, Tint.a,
                                                         OutlineMode);
    }
  } else {
    Renderer->DrawSprite(LabelSprite, Bounds, Tint);
  }
}

void Label::Move(glm::vec2 relativePosition) {
  for (ProcessedTextGlyph& glyph : Text) {
    glyph.Move(relativePosition);
  }
  Widget::Move(relativePosition);
}

void Label::SetSprite(Sprite const& label) {
  IsText = false;
  LabelSprite = label;
  Bounds = RectF(Bounds.X, Bounds.Y, LabelSprite.Bounds.Width,
                 LabelSprite.Bounds.Height);
}

void Label::SetText(std::vector<ProcessedTextGlyph>&& str, glm::vec2 pos,
                    RendererOutlineMode outlineMode, TextAlignment alignment) {
  IsText = true;
  OutlineMode = outlineMode;
  Text = std::move(str);

  if (Text.empty()) {
    Bounds = RectF(pos.x, pos.y, 0.0f, 0.0f);
  } else {
    RectF textBounds = GetTextBounds(Text);
    const glm::vec2 curTextPos = [alignment, textBounds]() {
      switch (alignment) {
        case TextAlignment::Left:
          return textBounds.TopLeft();
        case TextAlignment::Center:
          return glm::vec2(textBounds.Center().x, textBounds.Top());
        case TextAlignment::Right:
          return textBounds.TopRight();
      }
      assert(false);
      return textBounds.TopLeft();
    }();

    const glm::vec2 textOffset = pos - curTextPos;
    for (ProcessedTextGlyph& glyph : Text) {
      glyph.Move(textOffset);
    }
    textBounds += textOffset;

    // Preserve Bounds.GetPos() as being the position of the Label object
    Bounds.SetPos(pos);
    Bounds.SetSize(glm::max(textBounds.GetSize() + textBounds.GetPos() - pos,
                            glm::vec2(0.0f)));
  }
}

void Label::SetText(LabelStringType auto str, glm::vec2 pos, float fontSize,
                    RendererOutlineMode outlineMode,
                    DialogueColorPair colorPair, TextAlignment alignment) {
  std::vector<ProcessedTextGlyph> text;
  if constexpr (std::is_same_v<std::decay_t<decltype(str)>,
                               Vm::BufferOffsetContext>) {
    Impacto::Vm::Sc3VmThread dummy;
    dummy.IpOffset = str.IpOffset;
    dummy.ScriptBufferId = str.ScriptBufferId;
    text = TextLayoutPlainLine(&dummy, 255, *Profile::Dialogue::DialogueFont,
                               fontSize, colorPair, 1.0f, glm::vec2(0.0f),
                               TextAlignment::Left);
  } else if constexpr (std::is_convertible_v<decltype(str),
                                             const std::string_view>) {
    text = TextLayoutPlainString(str, *Profile::Dialogue::DialogueFont,
                                 fontSize, colorPair, 1.0f, glm::vec2(0.0f),
                                 TextAlignment::Left);
  } else {
    static_assert(std::is_same_v<std::decay_t<decltype(str)>, Vm::Sc3Stream>);
    text = TextLayoutPlainLine(str, 255, *Profile::Dialogue::DialogueFont,
                               fontSize, colorPair, 1.0f, glm::vec2(0.0f),
                               TextAlignment::Left);
  }

  SetText(std::move(text), pos, outlineMode, alignment);
}

template void Label::SetText(Vm::BufferOffsetContext, glm::vec2, float,
                             RendererOutlineMode, DialogueColorPair,
                             TextAlignment);
template void Label::SetText(Vm::Sc3Stream, glm::vec2, float,
                             RendererOutlineMode, DialogueColorPair,
                             TextAlignment);
template void Label::SetText(Vm::Sc3Stream&, glm::vec2, float,
                             RendererOutlineMode, DialogueColorPair,
                             TextAlignment);
template void Label::SetText(const std::string_view, glm::vec2, float,
                             RendererOutlineMode, DialogueColorPair,
                             TextAlignment);
template void Label::SetText(std::string, glm::vec2, float, RendererOutlineMode,
                             DialogueColorPair, TextAlignment);
template void Label::SetText(const char*, glm::vec2, float, RendererOutlineMode,
                             DialogueColorPair, TextAlignment);

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto