#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TitleMenu {

void Configure();

inline Sprite BackgroundSprite;
inline Sprite CopyrightSprite;
inline Sprite LogoSprite;
inline Sprite MenuBackgroundSprite;
inline Sprite MenuItemLockedSprite;
inline Sprite MenuItemLockedSpriteH;

inline float MenuEntriesX;
inline float MenuEntriesFirstY;
inline float MenuEntriesYPadding;
inline float MenuEntriesTargetWidth;
inline float LogoX;
inline float LogoY;
inline float CopyrightX;
inline float CopyrightY;
inline float PrimaryFadeAnimDuration;
inline glm::vec2 SecondaryMenuAnimTarget;
inline float SecondaryMenuPadding;
inline float SecondaryMenuAnimDuration;
inline float SecondaryMenuAnimUnderX;
inline float ExtraStoryItemCount;
inline float ContinueItemCount;
inline float MemoriesItemCount;
inline float SystemItemCount;

}  // namespace TitleMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto