#pragma once

#include "../../../hud/sysmesbox.h"
#include "../../../spritesheet.h"
#include "../../../games/rne/sysmesbox.h"

namespace Impacto {
namespace Profile {
namespace RNE {
namespace SysMesBox {

Impacto::SysMesBox::SysMesBoxBase* Configure();

extern Sprite BoxDecorationTop;
extern Sprite BoxDecorationBottom;
extern Sprite TextDecoration;
extern Sprite MessageLabel;
extern Sprite Line1;
extern Sprite Line2;
extern Sprite ButtonYes;
extern Sprite ButtonNo;
extern Sprite ButtonOK;

extern float LinePositionXFirst;
extern float LinePositionX;
extern float LinePositionMultiplier;
extern float LineWidthFirst;
extern float LineWidthBase;
extern float LineWidthMultiplier;
extern float Line1SpriteY;
extern float Line2SpriteY;
extern float LineSpriteHeight;
extern float LineDisplayXBase;
extern float Line1DisplayY;
extern float Line2DisplayY;
extern float BoxDisplayStartCount;
extern float BoxHeightBase;
extern float BoxHeightMultiplier;
extern float BoxWidth;
extern float BoxTextFontSize;
extern float BoxTopYBase;
extern float BoxDisplayX;
extern float MessageLabelSpriteXBase;
extern float MessageLabelSpriteY;
extern float MessageLabelSpriteHeight;
extern float MessageLabelSpriteMultiplier;
extern float ButtonYesDisplayXBase;
extern float ButtonRightDisplayXBase;
extern float ButtonSpriteY;
extern float ButtonWidth;
extern float ButtonSelectedSpriteY;
extern float ButtonYOffset;
extern float ButtonYWidthBase;
extern float ButtonRightWidthBase;
extern float TextDecorationStart;
extern float TextDecorationTopYOffset;
extern float TextDecorationBottomYOffset;
extern float SpriteMargin;

}  // namespace SysMesBox
}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto