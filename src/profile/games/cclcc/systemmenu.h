#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace SystemMenu {
constexpr inline int MenuEntriesNumMax = 16;
constexpr inline int GridRowCount = 10;
constexpr inline int GridColCount = 20;

inline Sprite SystemMenuBG;
inline Sprite MenuButtonGuide;
inline Sprite SystemMenuFrame;
inline Sprite SystemMenuMask;
inline float MoveInDuration;
inline float MoveOutDuration;
inline float ItemsFadeInDuration;
inline float ItemsFadeOutDuration;
inline glm::vec2 MenuEntriesPositions[MenuEntriesNumMax];
inline RectF MenuEntriesButtonBounds[MenuEntriesNumMax];

void Configure();

}  // namespace SystemMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto