#include "optionsbinarybutton.h"

#include "../../../profile/games/cclcc/optionsmenu.h"
#include "../../../renderer/renderer.h"
#include "../../../vm/interface/input.h"

using namespace Impacto::Profile::CCLCC::OptionsMenu;
using namespace Impacto::Vm::Interface;

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

OptionsBinaryButton::OptionsBinaryButton(const Sprite& box,
                                         const Sprite& trueLabel,
                                         const Sprite& falseLabel,
                                         const Sprite& label, glm::vec2 pos,
                                         glm::vec4 highlightTint)
    : OptionsEntry(label, pos, highlightTint),
      BoxSprite(box),
      TrueSprite(trueLabel),
      FalseSprite(falseLabel) {
  Bounds.Width = BinaryBoxOffset.x + BoxSprite.ScaledWidth();
}

inline glm::vec2 OptionsBinaryButton::GetTruePos() const {
  return Bounds.GetPos() + BinaryBoxOffset;
}

inline glm::vec2 OptionsBinaryButton::GetFalsePos() const {
  return Bounds.GetPos() + BinaryBoxOffset +
         glm::vec2(BoxSprite.ScaledWidth() / 2, 0.0f);
}

void OptionsBinaryButton::Render() {
  OptionsEntry::Render();

  RectF highlightBounds(0.0f, 0.0f, BoxSprite.ScaledWidth() / 2,
                        BoxSprite.ScaledHeight());
  glm::vec2 highlightPos = (State) ? GetTruePos() : GetFalsePos();
  highlightBounds.X = highlightPos.x;
  highlightBounds.Y = highlightPos.y;

  Renderer->DrawRect(highlightBounds, HighlightTint);
  Renderer->DrawSprite(BoxSprite, GetTruePos(), Tint);

  Renderer->DrawSprite(TrueSprite, GetTruePos(), Tint, glm::vec2(1.0f), 0.0f,
                       !State);
  Renderer->DrawSprite(FalseSprite, GetFalsePos(), Tint, glm::vec2(1.0f), 0.0f,
                       State);
}

void OptionsBinaryButton::UpdateInput() {
  OptionsEntry::UpdateInput();
  if (!Selected) return;

  if (PADinputButtonWentDown & (PAD1LEFT | PAD1RIGHT))
    State = PADinputButtonWentDown & PAD1LEFT;
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto