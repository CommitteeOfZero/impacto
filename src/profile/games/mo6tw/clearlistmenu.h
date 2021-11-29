#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace ClearListMenu {

extern Sprite BackgroundSprite;
extern glm::vec2 LabelPosition;
extern Sprite ClearListLabel;
extern glm::vec2 EndingsLabelPosition;
extern Sprite EndingsLabel;
extern glm::vec2 ScenesLabelPosition;
extern Sprite ScenesLabel;
extern glm::vec2 CompletionLabelPosition;
extern Sprite CompletionLabel;
extern glm::vec2 AlbumLabelPosition;
extern Sprite AlbumLabel;
extern glm::vec2 PlayTimeLabelPosition;
extern Sprite PlayTimeLabel;
extern Sprite ClearListTextBackground;
extern glm::vec2 ClearListTextBGOffset;

extern float FadeInDuration;
extern float FadeOutDuration;

void Configure();

}  // namespace ClearListMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto