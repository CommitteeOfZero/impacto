#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace SaveMenu {

int const EntriesPerRow = 2;
int const RowsPerPage = 4;

extern float EntryStartX;
extern float EntryXPadding;
extern float EntryStartY;
extern float EntryYPadding;

extern Sprite QuickLoadTextSprite;
extern Sprite LoadTextSprite;
extern Sprite SaveTextSprite;
extern glm::vec2 MenuTitleTextPos;

extern Sprite QuickLoadEntrySprite;
extern Sprite QuickLoadEntryHighlightedSprite;
extern Sprite SaveEntrySprite;
extern Sprite SaveEntryHighlightedSprite;
extern Sprite LoadEntrySprite;
extern Sprite LoadEntryHighlightedSprite;

void Configure();

}  // namespace SaveMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto