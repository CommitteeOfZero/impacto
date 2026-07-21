#pragma once

#include "../../../spritesheet.h"
#include "../../../games/cclcc/commonmenu.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace SystemMenu {

using namespace Impacto::UI::CCLCC;

constexpr inline int MenuEntriesNumMax = 16;

inline Sprite SystemMenuBG;
inline Sprite MenuButtonGuide;
inline Sprite SystemMenuFrame;
inline float MoveInDuration;
inline float MoveOutDuration;
inline float ItemsFadeInDuration;
inline float ItemsFadeOutDuration;
inline float FadeInDirectDuration;
inline float FadeOutDirectDuration;

inline Sprite OverlaySprite;

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
inline glm::vec2 BGRandPosRange;

inline glm::vec2 BGTranslationOffset;

inline float SmokeOpacityNormal;
inline float SmokeOpacitySystemMenu;
inline glm::vec2 SmokePosition;
inline std::array<Sprite, SmokeLayerCount> SmokeSprites;
inline std::array<float, SmokeLayerCount> SmokeAnimationDurations;

void Configure();

}  // namespace SystemMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto