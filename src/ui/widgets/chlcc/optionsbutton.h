#pragma once

#include "optionsentry.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

template <typename T>
class OptionsButton : public OptionsEntry {
 public:
  OptionsButton(
      T& value, std::span<const T> optionsValues,
      std::span<const std::reference_wrapper<const Sprite>> optionsSprites,
      glm::vec2 topRight, RectF hoverBounds,
      std::function<void(OptionsEntry*)> highlight);

  void Render() override;
  void UpdateInput(float dt) override;

 private:
  size_t GetCurrentOptionId();

  T& Value;

  std::span<const T> OptionsValues;
  std::span<const std::reference_wrapper<const Sprite>> OptionsSprites;
  ClickArea OptionClickArea;

  size_t OptionCount;
  size_t OptionId;

  glm::vec2 TopRight;
};

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
