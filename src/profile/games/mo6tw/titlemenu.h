#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TitleMenu {

void Configure();

extern Sprite BackgroundSprite;
extern Sprite CopyrightSprite;
extern Sprite LogoSprite;
extern Sprite MenuBackgroundSprite;
extern Sprite MenuItemLockedSprite;
extern Sprite MenuItemLockedSpriteH;

extern float MenuEntriesX;
extern float MenuEntriesFirstY;
extern float MenuEntriesYPadding;
extern float MenuEntriesTargetWidth;
extern float LogoX;
extern float LogoY;
extern float CopyrightX;
extern float CopyrightY;

}  // namespace TitleMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto