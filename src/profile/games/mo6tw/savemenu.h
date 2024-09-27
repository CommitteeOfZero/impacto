#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace SaveMenu {

int constexpr EntriesPerRow = 2;
int constexpr RowsPerPage = 4;

inline float EntryStartX;
inline float EntryXPadding;
inline float EntryStartY;
inline float EntryYPadding;

inline Sprite QuickLoadTextSprite;
inline Sprite LoadTextSprite;
inline Sprite SaveTextSprite;
inline glm::vec2 MenuTitleTextPos;

inline Sprite QuickLoadEntrySprite;
inline Sprite QuickLoadEntryHighlightedSprite;
inline Sprite SaveEntrySprite;
inline Sprite SaveEntryHighlightedSprite;
inline Sprite LoadEntrySprite;
inline Sprite LoadEntryHighlightedSprite;

inline Sprite SaveMenuBackgroundSprite;

void Configure();

}  // namespace SaveMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto