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

void Label::SetText(std::vector<ProcessedTextGlyph>&& str,
                    RendererOutlineMode outlineMode) {
  IsText = true;
  OutlineMode = outlineMode;
  Text = std::move(str);

  if (Text.empty()) {
    Bounds = RectF(Bounds.X, Bounds.Y, 0.0f, 0.0f);
  } else {
    for (ProcessedTextGlyph& glyph : Text) {
      glyph.DestRect += Bounds.GetPos();
    }

    const RectF textBounds =
        std::reduce(Text.begin() + 1, Text.end(), Text.front(),
                    [](auto lhs, const auto& rhs) {
                      lhs.DestRect =
                          RectF::Coalesce(lhs.DestRect, rhs.DestRect);
                      return lhs;
                    })
            .DestRect;

    // Preserve Bounds.GetPos() as being the position of the Label object
    Bounds.SetSize(
        glm::max(textBounds.GetSize() + textBounds.GetPos() - Bounds.GetPos(),
                 glm::vec2(0.0f)));
  }

  Bounds = Text.empty()
               ? RectF()
               : std::reduce(Text.begin() + 1, Text.end(), Text.front(),
                             [](auto lhs, const auto& rhs) {
                               lhs.DestRect =
                                   RectF::Coalesce(lhs.DestRect, rhs.DestRect);
                               return lhs;
                             })
                     .DestRect;
}

void Label::SetText(LabelStringType auto str, float fontSize,
                    RendererOutlineMode outlineMode,
                    DialogueColorPair colorPair) {
  std::vector<ProcessedTextGlyph> text;
  if constexpr (std::is_same_v<std::decay_t<decltype(str)>,
                               Vm::BufferOffsetContext>) {
    Impacto::Vm::Sc3VmThread dummy;
    dummy.IpOffset = str.IpOffset;
    dummy.ScriptBufferId = str.ScriptBufferId;
    text = TextLayoutPlainLine(&dummy, 255, *Profile::Dialogue::DialogueFont,
                               fontSize, colorPair, 1.0f, Bounds.GetPos(),
                               TextAlignment::Left);
  } else if constexpr (std::is_convertible_v<decltype(str),
                                             const std::string_view>) {
    text = TextLayoutPlainString(str, *Profile::Dialogue::DialogueFont,
                                 fontSize, colorPair, 1.0f, Bounds.GetPos(),
                                 TextAlignment::Left);
  } else {
    static_assert(std::is_same_v<std::decay_t<decltype(str)>, Vm::Sc3Stream>);
    text = TextLayoutPlainLine(str, 255, *Profile::Dialogue::DialogueFont,
                               fontSize, colorPair, 1.0f, Bounds.GetPos(),
                               TextAlignment::Left);
  }

  SetText(std::move(text), outlineMode);
}

template void Label::SetText(Vm::BufferOffsetContext, float,
                             RendererOutlineMode, DialogueColorPair);
template void Label::SetText(Vm::Sc3Stream, float, RendererOutlineMode,
                             DialogueColorPair);
template void Label::SetText(Vm::Sc3Stream&, float, RendererOutlineMode,
                             DialogueColorPair);
template void Label::SetText(const std::string_view, float, RendererOutlineMode,
                             DialogueColorPair);
template void Label::SetText(std::string, float, RendererOutlineMode,
                             DialogueColorPair);
template void Label::SetText(const char*, float, RendererOutlineMode,
                             DialogueColorPair);

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto