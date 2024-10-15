#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace SystemMenu {
constexpr int MenuEntriesNumMax = 16;

inline Sprite SystemMenuBG;
inline Sprite MenuButtonGuide;
inline Sprite SystemMenuFrame;
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