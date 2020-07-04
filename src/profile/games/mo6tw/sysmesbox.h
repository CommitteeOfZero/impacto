#pragma once

#include "../../../spritesheet.h"
#include "../../../games/mo6tw/sysmesbox.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace SysMesBox {

void Configure();

extern Sprite BoxPartLeft;
extern Sprite BoxPartRight;
extern Sprite BoxPartMiddle;
extern Sprite BoxDecoration;
extern Sprite SelectionHighlight;

extern float BoxX;
extern float BoxY;
extern float ChoicePadding;
extern float ChoiceY;
extern float ChoiceXBase;
extern float MinMaxMesWidth;
extern float BoxMinimumWidth;
extern float BoxMiddleBaseX;
extern float BoxMiddleBaseWidth;
extern float BoxMiddleRemainBase;
extern float BoxRightBaseX;
extern float BoxRightRemainPad;

}  // namespace SysMesBox
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto