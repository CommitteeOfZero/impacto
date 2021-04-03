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
extern float PrimaryFadeAnimDuration;
extern float SecondaryMenuAnimTargetX;
extern float SecondaryMenuAnimTargetY;
extern float SecondaryMenuAnimTarget2X;
extern float SecondaryMenuAnimTarget2Y;
extern float SecondaryMenuAnimDuration;
extern float SecondaryMenuAnimUnderX;
extern float SecondaryMenuAnimUnderYMultiplier;

}  // namespace TitleMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto