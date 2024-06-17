#pragma once

#include "../../../spritesheet.h"
#include "../../../games/mo8/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace MO8 {
namespace TitleMenu {

void Configure();

inline Sprite BackgroundSprite;
inline Sprite LogoSprite;

inline float LogoPositionX;
inline float LogoPositionY;

inline int NewGameSpriteIndex;
inline int ContinueSpriteIndex;
inline int OptionSpriteIndex;
inline int GallerySpriteIndex;
inline int AlbumSpriteIndex;
inline int MusicSpriteIndex;
inline int ClearListSpriteIndex;
inline int WarningSpriteIndex;
inline int AdditionalSpriteIndex;
inline int DLCSpriteIndex;
inline int LoadSpriteIndex;
inline int QuickLoadSpriteIndex;

inline float MenuEntriesX;
inline float MenuEntriesFirstY;
inline float MenuEntriesYPadding;
inline float MenuEntriesGalleryFirstY;

inline bool PressToStartAnimated;
inline bool HasAdditional;

inline Animation PressToStartAnimation;
inline float PrimaryFadeAnimDuration;
inline float ItemFadeAnimDuration;

}  // namespace TitleMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto