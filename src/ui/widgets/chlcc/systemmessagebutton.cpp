#include "systemmessagebutton.h"

#include "../../../profile/dialogue.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

SystemMessageButton::SystemMessageButton(int id, Sprite const& norm,
                                         Sprite const& focused,
                                         Sprite const& highlightLeft,
                                         Sprite const& highlightMiddle,
                                         Sprite const& highlightRight,
                                         glm::vec2 pos, RectF hoverBounds)
    : Button(id, norm, focused, highlightMiddle, pos, hoverBounds) {
  LeftHighlightSprite = highlightLeft;
  RightHighlightSprite = highlightRight;
}

void SystemMessageButton::Render() {
  if (HasFocus) {
    const RectF middleDest =
        HighlightSprite.ScaledBounds()
            .Scale({Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f},
                   {0.0f, 0.0f})
            .Translate(Bounds.GetPos());
    const RectF leftDest =
        LeftHighlightSprite.ScaledBounds().Translate(LeftHighlightPos);

    const RectF rightDest =
        RightHighlightSprite.ScaledBounds().Translate(RightHighlightPos);

    Renderer->DrawSprite(LeftHighlightSprite, leftDest, Tint);
    Renderer->DrawSprite(HighlightSprite, middleDest, Tint);
    Renderer->DrawSprite(RightHighlightSprite, rightDest, Tint);
  }

  if (HasText) {
    Renderer->DrawProcessedText(Text, Profile::Dialogue::DialogueFont, Tint.a,
                                OutlineMode, true);
  }
}

void SystemMessageButton::SetText(std::vector<ProcessedTextGlyph> text,
                                  float textWidth, float fontSize,
                                  RendererOutlineMode outlineMode) {
  HasText = true;
  Text = std::move(text);
  TextWidth = textWidth;
  OutlineMode = outlineMode;
  HighlightOffset = glm::vec2(0.0f, -1.0f);

  Bounds = RectF(Text[0].DestRect.X, Text[0].DestRect.Y, TextWidth, fontSize) +
           HighlightOffset;

  const glm::vec2 leftSize = LeftHighlightSprite.ScaledBounds().GetSize();
  const float leftWidth = leftSize.x * (fontSize / leftSize.y);

  const glm::vec2 rightSize = RightHighlightSprite.ScaledBounds().GetSize();
  const float rightWidth = rightSize.x * (fontSize / rightSize.y);

  LeftHighlightPos = {Bounds.X - leftWidth - 0.5f, Bounds.Y};
  RightHighlightPos = {Bounds.X + TextWidth, Bounds.Y};
  HoverBounds = RectF(LeftHighlightPos.x, Bounds.Y,
                      TextWidth + leftWidth + rightWidth, fontSize);
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto