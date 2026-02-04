#include "optionsbutton.h"

#include "../../../profile/games/chlcc/optionsmenu.h"
#include "../../../renderer/renderer.h"
#include "../../../inputsystem.h"
#include "../../../vm/interface/input.h"
#include "../../../log.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

using namespace Impacto::Vm::Interface;
using namespace Impacto::Profile::CHLCC::OptionsMenu;

template <typename T>
OptionsButton<T>::OptionsButton(
    T& value, const std::span<const T> optionsValues,
    const std::span<const std::reference_wrapper<const Sprite>> optionsSprites,
    const glm::vec2 topRight, const RectF hoverBounds,
    const std::function<void(OptionsEntry*)> highlight)
    : OptionsEntry(hoverBounds, highlight),
      Value(value),
      OptionsValues(optionsValues),
      OptionsSprites(optionsSprites),
      OptionClickArea(0, hoverBounds,
                      [this](ClickArea* area) {
                        this->OptionId =
                            (this->OptionId + 1) % this->OptionCount;
                        this->Value = this->OptionsValues[this->OptionId];
                      }),
      OptionCount(optionsValues.size()),
      TopRight(topRight) {
  assert(OptionsValues.size() == OptionsSprites.size());
  OptionId = GetCurrentOptionId();
}

template <typename T>
void OptionsButton<T>::Render() {
  const Sprite& optionSprite = OptionsSprites[OptionId];
  Renderer->DrawSprite(optionSprite,
                       TopRight + SettingButtonTopRightOffset -
                           glm::vec2(optionSprite.Bounds.Width, 0.0f));
}

template <typename T>
void OptionsButton<T>::UpdateInput(float dt) {
  OptionsEntry::UpdateInput(dt);

  OptionClickArea.UpdateInput(dt);

  if (!HasFocus) return;

  const int direction =
      static_cast<int>(
          static_cast<bool>(PADinputButtonWentDown & (PAD1RIGHT | PAD1A))) -
      static_cast<int>(static_cast<bool>(PADinputButtonWentDown & PAD1LEFT));

  if (direction != 0) {
    OptionId = (OptionId + direction + OptionCount) % OptionCount;
    Value = OptionsValues[OptionId];
  }
}

template <typename T>
void OptionsButton<T>::Move(glm::vec2 relativePos) {
  OptionsEntry::Move(relativePos);

  TopRight += relativePos;
  EntryButton.Move(relativePos);
}

template <typename T>
size_t OptionsButton<T>::GetCurrentOptionId() {
  const auto found = std::ranges::find(OptionsValues, Value);

  if (found == OptionsValues.end()) {
    ImpLog(LogLevel::Error, LogChannel::General,
           "Failed to find option id. Replacing with first option\n");
    Value = OptionsValues[0];
    return 0;
  }

  return std::distance(OptionsValues.begin(), found);
}

template <>
size_t OptionsButton<float>::GetCurrentOptionId() {
  const auto shorterDistance = [&](float lhs, float rhs) {
    return std::abs(lhs - Value) < std::abs(rhs - Value);
  };
  const auto closestValue =
      std::ranges::min_element(OptionsValues, shorterDistance);
  return std::distance(OptionsValues.begin(), closestValue);
}

template class OptionsButton<bool>;
template class OptionsButton<uint8_t>;
template class OptionsButton<float>;

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
