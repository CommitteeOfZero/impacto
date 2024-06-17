#pragma once

#include "../../../spritesheet.h"
#include "../../../games/rne/sysmesbox.h"

namespace Impacto {
namespace Profile {
namespace RNE {
namespace SysMesBox {

void Configure();

inline Sprite BoxDecorationTop;
inline Sprite BoxDecorationBottom;
inline Sprite TextDecoration;
inline Sprite MessageLabel;
inline Sprite Line1;
inline Sprite Line2;
inline Sprite ButtonYes;
inline Sprite ButtonNo;
inline Sprite ButtonOK;
inline Sprite ButtonYesHighlighted;
inline Sprite ButtonNoHighlighted;
inline Sprite ButtonOKHighlighted;

inline float LinePositionXFirst;
inline float LinePositionX;
inline float LinePositionMultiplier;
inline float LineWidthFirst;
inline float LineWidthBase;
inline float LineWidthMultiplier;
inline float Line1SpriteY;
inline float Line2SpriteY;
inline float LineSpriteHeight;
inline float LineDisplayXBase;
inline float Line1DisplayY;
inline float Line2DisplayY;
inline float BoxDisplayStartCount;
inline float BoxHeightBase;
inline float BoxHeightMultiplier;
inline float BoxWidth;
inline float BoxTextFontSize;
inline float BoxTopYBase;
inline float BoxDisplayX;
inline float MessageLabelSpriteXBase;
inline float MessageLabelSpriteY;
inline float MessageLabelSpriteHeight;
inline float MessageLabelSpriteMultiplier;
inline float ButtonYesDisplayXBase;
inline float ButtonRightDisplayXBase;
inline float ButtonWidth;
inline float ButtonSelectedSpriteY;
inline float ButtonYOffset;
inline float ButtonYWidthBase;
inline float ButtonRightWidthBase;
inline float TextDecorationStart;
inline float TextDecorationTopYOffset;
inline float TextDecorationBottomYOffset;

}  // namespace SysMesBox
}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto