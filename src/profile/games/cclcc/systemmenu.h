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

inline glm::vec2 BGDispOffsetTopLeft;
inline glm::vec2 BGDispOffsetBottomLeft;
inline glm::vec2 BGDispOffsetTopRight;
inline glm::vec2 BGDispOffsetBottomRight;

inline glm::vec2 FrameOffsetTopLeft;
inline glm::vec2 FrameOffsetBottomLeft;
inline glm::vec2 FrameOffsetTopRight;
inline glm::vec2 FrameOffsetBottomRight;

inline glm::vec3 AngleMultiplier;
inline glm::vec2 BGRandPosInit;
inline glm::vec2 BGRandPosRange;

inline glm::vec2 BGTranslationOffset;
void Configure();

}  // namespace SystemMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto