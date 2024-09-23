#pragma once

#include "../../../spritesheet.h"
#include "../../../games/sghd/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace SGHD {
namespace TitleMenu {

void Configure();

extern Sprite BackgroundSprite;
extern float BackgroundX;
extern float BackgroundY;

extern float PressToStartAnimFastDurationIn;
extern float PressToStartAnimFastDurationOut;

extern float MenuX;
extern float MenuY;
extern float MenuEntriesSpacingY;

extern int MenuStartId;
extern int MenuLoadId;
extern int MenuExtraId;
extern int MenuConfigId;
extern int MenuHelpId;

// TODO: All the other sprites

}  // namespace TitleMenu
}  // namespace SGHD
}  // namespace Profile
}  // namespace Impacto
