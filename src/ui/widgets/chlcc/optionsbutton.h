#pragma once

#include "optionsentry.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

class OptionsButton : public OptionsEntry {
 public:
  OptionsButton(std::span<const Sprite*> optionsSprites, glm::vec2 topRight,
                RectF hoverBounds,
                std::function<void(OptionsEntry*)> highlight);

  void Render() override;
  void UpdateInput(float dt) override;

 private:
  void SetOption(int optionId);

  std::vector<const Sprite*> OptionsSprites;
  ClickArea OptionClickArea;

  size_t OptionId = 0;
  size_t OptionCount;

  glm::vec2 TopRight;
};

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
