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
extern int GallerySpriteIndex;
extern int AlbumSpriteIndex;
extern int MusicSpriteIndex;
extern int ClearListSpriteIndex;
extern int WarningSpriteIndex;
extern int AdditionalSpriteIndex;
extern int DLCSpriteIndex;
extern int LoadSpriteIndex;
extern int QuickLoadSpriteIndex;

extern float MenuEntriesX;
extern float MenuEntriesFirstY;
extern float MenuEntriesYPadding;
extern float MenuEntriesGalleryFirstY;

extern bool PressToStartAnimated;
extern bool HasAdditional;

extern Animation PressToStartAnimation;
extern float PrimaryFadeAnimDuration;
extern float ItemFadeAnimDuration;

}  // namespace TitleMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto