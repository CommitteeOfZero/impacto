#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO8 {
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

inline Sprite NextButtonSprite;
inline Sprite NextButtonHighlightedSprite;
inline glm::vec2 NextButtonPosition;
inline Sprite BackButtonSprite;
inline Sprite BackButtonHighlightedSprite;
inline glm::vec2 BackButtonPosition;

void Configure();

}  // namespace SaveMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto