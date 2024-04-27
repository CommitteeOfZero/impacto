#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO8 {
namespace SaveMenu {

int const EntriesPerRow = 2;
int const RowsPerPage = 4;

extern Sprite QuickLoadTextSprite;
extern Sprite LoadTextSprite;
extern Sprite SaveTextSprite;
extern glm::vec2 MenuTitleTextPos;

extern Sprite NextButtonSprite;
extern Sprite NextButtonHighlightedSprite;
extern glm::vec2 NextButtonPosition;
extern Sprite BackButtonSprite;
extern Sprite BackButtonHighlightedSprite;
extern glm::vec2 BackButtonPosition;

void Configure();

}  // namespace SaveMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto