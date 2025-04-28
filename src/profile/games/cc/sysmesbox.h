#pragma once

#include "../../../spritesheet.h"
#include "../../../games/mo6tw/sysmesbox.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace SysMesBox {

int constexpr SealSpriteCount = 8;
inline Sprite SumoSealSprites[SealSpriteCount];

inline Sprite ButtonYes;
inline Sprite ButtonNo;
inline Sprite ButtonOK;
inline Sprite ButtonYesHighlighted;
inline Sprite ButtonNoHighlighted;
inline Sprite ButtonOKHighlighted;

inline float SumoSealCenterPosX[SealSpriteCount];
inline float SumoSealCenterPosY[SealSpriteCount];
inline float ButtonYesCenterPosX;
inline float ButtonYesCenterPosY;
inline float ButtonNoCenterPosX;
inline float ButtonNoCenterPosY;
inline float ButtonOKCenterPosX;
inline float ButtonOKCenterPosY;
inline float AnimationProgressWidgetsStartOffset;
inline float ButtonNoDisplayStart;
inline float ButtonNoAnimationProgressOffset;
inline float ButtonYesNoScaleMultiplier;
inline float ButtonOKScaleMultiplier;
inline float ButtonScaleMax;
inline float ButtonYesAnimationProgressEnd;
inline float ButtonYesNoAlphaDivider;
inline float WidgetsAlphaMultiplier;

inline RectF ButtonYesHoverBounds;
inline RectF ButtonNoHoverBounds;
inline RectF ButtonOkHoverBounds;

void Configure();

}  // namespace SysMesBox
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto