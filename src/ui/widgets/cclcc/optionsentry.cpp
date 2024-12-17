#include "optionsentry.h"

#include "../../../profile/games/cclcc/optionsmenu.h"
#include "../../../renderer/renderer.h"
#include "../../../vm/interface/input.h"

using namespace Impacto::Profile::CCLCC::OptionsMenu;
using namespace Impacto::Vm::Interface;

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

OptionsEntry::OptionsEntry(const Sprite& label, glm::vec2 pos,
                           glm::vec4 highlightTint)
    : LabelSprite(label), HighlightTint(highlightTint) {
  Bounds = RectF(pos.x, pos.y, LabelSprite.ScaledWidth(),
                 LabelSprite.ScaledHeight());
}

void OptionsEntry::Render() {
  HighlightTint.a = Tint.a;

  if (HasFocus) {
    RectF highlightBoundBox(Bounds.X, Bounds.Y, EntryDimensions.x,
                            EntryDimensions.y);
    Renderer->DrawRect(highlightBoundBox, HighlightTint);
    Renderer->DrawRect(highlightBoundBox + RectF(2.0f, 2.0f, -4.0f, -4.0f),
                       glm::vec4(1.0f, 1.0f, 1.0f, Tint.a));

    Renderer->DrawSprite(PointerSprite, Bounds.GetPos() + PointerOffset, Tint);
  }

  Renderer->DrawSprite(LabelSprite, Bounds.GetPos() + LabelOffset,
                       Selected ? Tint : glm::vec4(0.0f, 0.0f, 0.0f, Tint.a));
}

void OptionsEntry::UpdateInput() {
  if (!HasFocus) return;

  Selected ^= (bool)(PADinputButtonWentDown & PAD1A);
  if (PADinputButtonWentDown & PAD1B) Selected = false;
}

void OptionsEntry::Hide() {
  Widget::Hide();
  Selected = false;
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto