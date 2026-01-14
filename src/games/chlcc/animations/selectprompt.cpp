#include "selectprompt.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

SelectPromptAnimation::SelectPromptAnimation() {
  Direction = AnimationDirection::In;
  LoopMode = AnimationLoopMode::Loop;
  DurationIn = MenuSelectPromptDuration;
  DurationOut = 0.0f;
}

void SelectPromptAnimation::Draw(const std::span<const Sprite> sprites,
                                 const std::span<const glm::vec2> positions,
                                 const glm::vec2 offset) {
  const auto currentLetter = static_cast<size_t>(
      Progress * MenuSelectPromptDuration / MenuSelectPromptInterval);
  for (size_t i = 0; i < sprites.size(); i++) {
    if (currentLetter < i) break;
    const glm::vec2 pos = positions[i];
    const float alpha = i == currentLetter
                            ? (Progress * MenuSelectPromptDuration -
                               i * MenuSelectPromptInterval) /
                                  MenuSelectPromptInterval
                            : 1.0f;
    Renderer->DrawSprite(sprites[i], pos + offset,
                         glm::vec4(1.0f, 1.0f, 1.0f, alpha));
  }
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto