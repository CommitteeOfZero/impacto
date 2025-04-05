#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace ClearListMenu {

int constexpr Endings = 10;

inline Sprite ClearListBookLayerSprite;
inline Sprite ClearListCompleteBookLayerSprite;
inline Sprite ClearListGuideSprite;
inline Sprite ClearListMaskSprite;

inline Sprite EndingSprites[Endings];
inline float EndingSpriteOffsetY;
inline float MenuOffsetY;

inline float FadeInDuration;
inline float FadeOutDuration;
inline glm::vec2 ClearListGuidePosition;

void Configure();

}  // namespace ClearListMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto