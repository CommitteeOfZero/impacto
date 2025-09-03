#include "optionsbutton.h"

#include "../../../profile/games/chlcc/optionsmenu.h"
#include "../../../renderer/renderer.h"
#include "../../../inputsystem.h"
#include "../../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

using namespace Impacto::Vm::Interface;
using namespace Impacto::Profile::CHLCC::OptionsMenu;

OptionsButton::OptionsButton(const std::span<const Sprite*> optionsSprites,
                             const glm::vec2 topRight, const RectF hoverBounds,
                             const std::function<void(OptionsEntry*)> highlight)
    : OptionsEntry(hoverBounds, highlight),
      OptionsSprites(optionsSprites.begin(), optionsSprites.end()),
      OptionClickArea(0, hoverBounds,
                      [this](ClickArea* area) {
                        this->OptionId =
                            (this->OptionId + 1) % this->OptionCount;
                      }),
      OptionCount(optionsSprites.size()),
      TopRight(topRight) {}

void OptionsButton::Render() {
  const Sprite* optionSprite = OptionsSprites[OptionId];
  Renderer->DrawSprite(*optionSprite,
                       TopRight + SettingButtonTopRightOffset -
                           glm::vec2(optionSprite->Bounds.Width, 0.0f));
}

void OptionsButton::UpdateInput(float dt) {
  OptionsEntry::UpdateInput(dt);

  OptionClickArea.UpdateInput(dt);

  if (!HasFocus) return;

  const int direction =
      static_cast<int>(
          static_cast<bool>(PADinputButtonWentDown & (PAD1RIGHT | PAD1A))) -
      static_cast<int>(static_cast<bool>(PADinputButtonWentDown & PAD1LEFT));

  if (direction != 0) {
    OptionId = (OptionId + direction + OptionCount) % OptionCount;
  }
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
