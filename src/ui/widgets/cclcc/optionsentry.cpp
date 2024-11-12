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

  Renderer->DrawSprite(LabelSprite, Bounds.GetPos(),
                       Selected ? Tint : glm::vec4(0.0f, 0.0f, 0.0f, Tint.a));
  if (HasFocus)
    Renderer->DrawSprite(PointerSprite, Bounds.GetPos() + PointerOffset, Tint);
}

void OptionsEntry::UpdateInput() {
  if (!HasFocus) return;

  if (PADinputButtonWentDown & PAD1A) {
    Selected = true;
  } else if (PADinputButtonWentDown & PAD1B) {
    Selected = false;
  }
}

void OptionsEntry::Hide() { Selected = false; }

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto