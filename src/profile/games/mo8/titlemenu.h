#pragma once

#include "../../../spritesheet.h"
#include "../../../games/mo8/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace MO8 {
namespace TitleMenu {

void Configure();

extern Sprite BackgroundSprite;
extern Sprite LogoSprite;

extern float LogoPositionX;
extern float LogoPositionY;

extern int NewGameSpriteIndex;
extern int ContinueSpriteIndex;
extern int OptionSpriteIndex;

extern float MenuEntriesX;
extern float MenuEntriesFirstY;
extern float MenuEntriesYPadding;

extern bool PressToStartAnimated;

extern Animation PressToStartAnimation;

}  // namespace TitleMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto