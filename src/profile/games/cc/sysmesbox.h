#pragma once

#include "../../../spritesheet.h"
#include "../../../games/mo6tw/sysmesbox.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace SysMesBox {

int const SealSpriteCount = 8;
extern Sprite SumoSealSprites[SealSpriteCount];

extern Sprite ButtonYes;
extern Sprite ButtonNo;
extern Sprite ButtonOK;
extern Sprite ButtonYesHighlighted;
extern Sprite ButtonNoHighlighted;
extern Sprite ButtonOKHighlighted;

extern float SumoSealCenterPosX[SealSpriteCount];
extern float SumoSealCenterPosY[SealSpriteCount];
extern float ButtonYesCenterPosX;
extern float ButtonYesCenterPosY;
extern float ButtonNoCenterPosX;
extern float ButtonNoCenterPosY;
extern float ButtonOKCenterPosX;
extern float ButtonOKCenterPosY;
extern float AnimationProgressWidgetsStartOffset;
extern float ButtonNoDisplayStart;
extern float ButtonNoAnimationProgressOffset;
extern float ButtonYesNoScaleMultiplier;
extern float ButtonOKScaleMultiplier;
extern float ButtonScaleMax;
extern float ButtonYesAnimationProgressEnd;
extern float ButtonYesNoAlphaDivider;
extern float WidgetsAlphaMultiplier;

void Configure();

}  // namespace SysMesBox
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto