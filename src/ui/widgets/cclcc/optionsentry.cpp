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
                           glm::vec4 highlightTint,
                           std::function<void(OptionsEntry*)> select)
    : LabelSprite(label), HighlightTint(highlightTint), Select(select) {
  Bounds = RectF(pos.x, pos.y, LabelOffset.x + LabelSprite.ScaledWidth(),
                 LabelOffset.y + LabelSprite.ScaledHeight());

  std::function<void(ClickButton*)> onClick =
      std::bind(&OptionsEntry::EntryButtonOnClick, this, std::placeholders::_1);
  EntryButton = ClickButton(0, Bounds, onClick);
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
  EntryButton.UpdateInput();

  if (!HasFocus) return;

  Selected ^= (bool)(PADinputButtonWentDown & PAD1A);
  if (PADinputButtonWentDown & PAD1B || PADinputMouseWentDown & PAD1B)
    Selected = false;
}

void OptionsEntry::Hide() {
  Widget::Hide();
  Selected = false;
}

void OptionsEntry::EntryButtonOnClick(ClickButton* target) { Select(this); }

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto