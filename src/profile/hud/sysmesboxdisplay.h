#pragma once

#include "../sprites.h"

namespace Impacto {
namespace Profile {
namespace SysMesBoxDisplay {

extern Sprite BoxDecorationTop;
extern Sprite BoxDecorationBottom;
extern Sprite TextDecoration;
extern Sprite MessageLabel;
extern Sprite Line1;
extern Sprite Line2;

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
extern float AnimationSpeed;
extern float FadeInDuration;
extern float FadeOutDuration;

void Configure();

}  // namespace SysMesBoxDisplay
}  // namespace Profile
}  // namespace Impacto