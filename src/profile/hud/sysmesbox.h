#pragma once

#include "../../hud/sysmesbox.h"
#include "../sprites.h"

namespace Impacto {
namespace Profile {
namespace SysMesBox {

extern Impacto::SysMesBox::SysMesBoxType Type;

extern float TextFontSize;
extern float TextMiddleY;
extern float TextX;
extern float TextLineHeight;
extern float TextMarginY;
extern float AnimationSpeed;
extern float FadeInDuration;
extern float FadeOutDuration;

void Configure();

}  // namespace SysMesBoxDisplay
}  // namespace Profile
}  // namespace Impacto