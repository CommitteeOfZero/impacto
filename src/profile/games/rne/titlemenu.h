#pragma once

#include "../../../spritesheet.h"
#include "../../../games/rne/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace RNE {
namespace TitleMenu {

void Configure();

extern Sprite LineSprite;
extern Sprite CopyrightSprite;
extern Sprite EliteSprite;
extern Sprite LogoSprite;

extern float PreTitleAnimDurationIn;
extern float PreTitleAnimDurationOut;

extern float LineWidth;
extern float CopyrightWidth;
extern float EliteHeight;
extern float LogoWidth;
extern float LineX;
extern float LineY;
extern float CopyrightX;
extern float CopyrightY;
extern float EliteX;
extern float EliteY;
extern float LogoX;
extern float LogoY;

extern Animation* BackgroundAnimation;
extern Animation PreTitleItemsAnimation;
extern Animation PressToStartAnimation;

}  // namespace TitleMenu
}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto