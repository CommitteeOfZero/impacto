#pragma once

#include "../../../spritesheet.h"
#include <enum.h>

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace SaveMenu {
BETTER_ENUM(SaveMenuPageType, int, QuickLoad, Save, Load)

int const static Pages = 6;

extern float EntryStartXL;
extern float EntryStartXR;
extern float EntryStartYL;
extern float EntryStartYR;
extern float EntryYPadding;

int const static EntriesPerRow = 2;
int const static RowsPerPage = 4;
extern Sprite BackgroundSprite;

extern Sprite MenuTextSprite[3];
extern Sprite EntryHighlightedBoxSprite[3];
extern Sprite EntryHighlightedTextSprite[3];
extern Sprite EntrySlotsSprite[3];
extern Sprite ButtonGuideSprite[3];
extern Sprite NumberDigitSprite[3][10];
extern Sprite NoDataSprite[3];
extern Sprite BrokenDataSprite[3];
extern Sprite SlotLockedSprite[3];
extern Sprite PageNumSprite[3][6];
extern Sprite SaveTimeSprite[3];

void Configure();

}  // namespace SaveMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto