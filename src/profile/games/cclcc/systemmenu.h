#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace SystemMenu {
constexpr int MenuEntriesNumMax = 16;

extern Sprite MenuButtonGuide;
extern float TitleFadeInDuration;
extern float TitleFadeOutDuration;
extern glm::vec2 MenuEntriesPositions[MenuEntriesNumMax];

void Configure();

}  // namespace SystemMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto