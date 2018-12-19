#pragma once

#include "../../hud/titlemenu.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace TitleMenu {

extern Impacto::TitleMenu::TitleMenuType Type;

extern Sprite PressToStartSprite;
extern Sprite LineSprite;
extern Sprite CopyrightSprite;
extern Sprite EliteSprite;
extern Sprite LogoSprite;

extern float PreTitleAnimDurationIn;
extern float PreTitleAnimDurationOut;
extern float PressToStartAnimDurationIn;
extern float PressToStartAnimDurationOut;

extern float LineWidth;
extern float CopyrightWidth;
extern float EliteHeight;
extern float LogoWidth;
extern float PressToStartX;
extern float PressToStartY;
extern float LineX;
extern float LineY;
extern float CopyrightX;
extern float CopyrightY;
extern float EliteX;
extern float EliteY;
extern float LogoX;
extern float LogoY;

void Configure();

}  // namespace TitleMenu
}  // namespace Profile
}  // namespace Impacto