#pragma once

#include "../../../spritesheet.h"
#include "../../../ui/savemenu.h"
#include <enum.h>

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace SaveMenu {

int constexpr Pages = 6;

inline float EntryStartXL;
inline float EntryStartXR;
inline float EntryStartYL;
inline float EntryStartYR;
inline float EntryYPadding;

int constexpr EntriesPerRow = 2;
int constexpr RowsPerPage = 4;
inline Sprite SaveMenuMaskSprite;
inline uint32_t SaveEntryPrimaryColor;
inline uint32_t SaveEntrySecondaryColor;

inline Sprite MenuTextSprite[UI::SaveMenuPageType::_size_constant];
inline Sprite EntryHighlightedBoxSprite[UI::SaveMenuPageType::_size_constant];
inline Sprite EntryHighlightedTextSprite[UI::SaveMenuPageType::_size_constant];
inline Sprite EntrySlotsSprite[UI::SaveMenuPageType::_size_constant];
inline Sprite ButtonGuideSprite[UI::SaveMenuPageType::_size_constant];
inline Sprite SeparationLineSprite[UI::SaveMenuPageType::_size_constant];
inline Sprite NumberDigitSprite[UI::SaveMenuPageType::_size_constant][10];
inline Sprite NoDataSprite[UI::SaveMenuPageType::_size_constant];
inline Sprite BrokenDataSprite[UI::SaveMenuPageType::_size_constant];
inline Sprite SlotLockedSprite[UI::SaveMenuPageType::_size_constant];
inline Sprite PageNumSprite[UI::SaveMenuPageType::_size_constant][6];
inline Sprite SaveTimeSprite[UI::SaveMenuPageType::_size_constant];

void Configure();

}  // namespace SaveMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto