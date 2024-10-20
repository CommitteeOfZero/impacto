#include "optionsbinarybutton.h"

#include "../../../profile/games/cclcc/optionsmenu.h"
#include "../../../renderer/renderer.h"

using namespace Impacto::Profile::CCLCC::OptionsMenu;

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

OptionsBinaryButton::OptionsBinaryButton(const Sprite& box,
                                         const Sprite& trueLabel,
                                         const Sprite& falseLabel,
                                         const Sprite& label, glm::vec2 pos)
    : BoxSprite(box),
      TrueSprite(trueLabel),
      FalseSprite(falseLabel),
      LabelSprite(label) {
  Bounds = RectF(pos.x, pos.y, BinaryBoxOffset.x + BoxSprite.ScaledWidth(),
                 LabelSprite.ScaledHeight());
}

inline glm::vec2 OptionsBinaryButton::GetTruePos() const {
  return Bounds.GetPos() + BinaryBoxOffset;
}

inline glm::vec2 OptionsBinaryButton::GetFalsePos() const {
  return Bounds.GetPos() + BinaryBoxOffset +
         glm::vec2(BoxSprite.ScaledWidth() / 2, 0.0f);
}

void OptionsBinaryButton::Render() {
  HighlightTint.a = Tint.a;
  glm::vec4 black = glm::vec4(0.0f, 0.0f, 0.0f, Tint.a);

  glm::vec4 trueTint;
  glm::vec4 falseTint;
  glm::vec2 highlightPos;
  RectF highlightBounds(0.0f, 0.0f, BoxSprite.ScaledWidth() / 2,
                        BoxSprite.ScaledHeight());
  if (State) {
    trueTint = Tint;
    falseTint = black;
    highlightPos = GetTruePos();
  } else {
    trueTint = black;
    falseTint = Tint;
    highlightPos = GetFalsePos();
  }
  highlightBounds.X = highlightPos.x;
  highlightBounds.Y = highlightPos.y;

  Renderer->DrawSprite(LabelSprite, Bounds.GetPos(), black);

  Renderer->DrawRect(highlightBounds, HighlightTint);
  Renderer->DrawSprite(BoxSprite, GetTruePos(), Tint);

  Renderer->DrawSprite(TrueSprite, GetTruePos(), trueTint);
  Renderer->DrawSprite(FalseSprite, GetFalsePos(), falseTint);
}

void OptionsBinaryButton::UpdateInput() {}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto