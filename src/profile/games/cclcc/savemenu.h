#pragma once

#include "../../../spritesheet.h"
#include <enum.h>

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace SaveMenu {
BETTER_ENUM(SaveMenuPageType, int, QuickLoad, Save, Load)

int constexpr Pages = 6;

inline float EntryStartXL;
inline float EntryStartXR;
inline float EntryStartYL;
inline float EntryStartYR;
inline float EntryYPadding;

int constexpr EntriesPerRow = 2;
int constexpr RowsPerPage = 4;
inline Sprite BackgroundSprite;

inline Sprite MenuTextSprite[SaveMenuPageType::_size_constant];
inline Sprite EntryHighlightedBoxSprite[SaveMenuPageType::_size_constant];
inline Sprite EntryHighlightedTextSprite[SaveMenuPageType::_size_constant];
inline Sprite EntrySlotsSprite[SaveMenuPageType::_size_constant];
inline Sprite ButtonGuideSprite[SaveMenuPageType::_size_constant];
inline Sprite NumberDigitSprite[SaveMenuPageType::_size_constant][10];
inline Sprite NoDataSprite[SaveMenuPageType::_size_constant];
inline Sprite BrokenDataSprite[SaveMenuPageType::_size_constant];
inline Sprite SlotLockedSprite[SaveMenuPageType::_size_constant];
inline Sprite PageNumSprite[SaveMenuPageType::_size_constant][6];
inline Sprite SaveTimeSprite[SaveMenuPageType::_size_constant];

void Configure();

}  // namespace SaveMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto