#pragma once

#include "../../../spritesheet.h"
#include "../../../games/rne/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace RNE {
namespace TitleMenu {

void Configure();

inline Sprite LineSprite;
inline Sprite CopyrightSprite;
inline Sprite EliteSprite;
inline Sprite LogoSprite;

inline float PreTitleAnimDurationIn;
inline float PreTitleAnimDurationOut;

inline float LineWidth;
inline float CopyrightWidth;
inline float EliteHeight;
inline float LogoWidth;
inline float LineX;
inline float LineY;
inline float CopyrightX;
inline float CopyrightY;
inline float EliteX;
inline float EliteY;
inline float LogoX;
inline float LogoY;

inline Animation* BackgroundAnimation = nullptr;
inline Animation PreTitleItemsAnimation;
inline Animation PressToStartAnimation;

}  // namespace TitleMenu
}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto