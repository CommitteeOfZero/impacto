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
                                         const Sprite& label, glm::vec2 pos,
                                         glm::vec4 highlightTint)
    : BoxSprite(box),
      TrueSprite(trueLabel),
      FalseSprite(falseLabel),
      LabelSprite(label),
      HighlightTint(highlightTint) {
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

  RectF highlightBounds(0.0f, 0.0f, BoxSprite.ScaledWidth() / 2,
                        BoxSprite.ScaledHeight());
  glm::vec2 highlightPos = (State) ? GetTruePos() : GetFalsePos();
  highlightBounds.X = highlightPos.x;
  highlightBounds.Y = highlightPos.y;

  Renderer->DrawSprite(LabelSprite, Bounds.GetPos(),
                       {0.0f, 0.0f, 0.0f, Tint.a});

  Renderer->DrawRect(highlightBounds, HighlightTint);
  Renderer->DrawSprite(BoxSprite, GetTruePos(), Tint);

  Renderer->DrawSprite(TrueSprite, GetTruePos(), Tint, glm::vec2(1.0f), 0.0f,
                       !State);
  Renderer->DrawSprite(FalseSprite, GetFalsePos(), Tint, glm::vec2(1.0f), 0.0f,
                       State);
}

void OptionsBinaryButton::UpdateInput() {}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto