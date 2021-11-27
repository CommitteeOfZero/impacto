#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TipsMenu {

int const ThumbnailCount = 37;
int const MaxCategoryString = 5;

extern Sprite TipThumbnails[ThumbnailCount];
extern Sprite TipTextOnlyThumbnail;
extern uint16_t CategoryString[MaxCategoryString];

void Configure();

}  // namespace TipsMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto