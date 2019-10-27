#pragma once

#include "../../../hud/sysmesbox.h"
#include "../../../spritesheet.h"
#include "../../../games/darling/sysmesbox.h"

namespace Impacto {
namespace Profile {
namespace Darling {
namespace SysMesBox {

Impacto::SysMesBox::SysMesBoxBase* Configure();

extern Sprite BoxPartLeft;
extern Sprite BoxPartRight;
extern Sprite BoxPartMiddle;
extern Sprite BoxDecoration;

extern float BoxX;
extern float BoxY;
extern float BoxMinimumWidth;
extern float BoxMiddleBaseX;
extern float BoxMiddleBaseWidth;
extern float BoxRightBaseWidth;
extern float BoxRightRemainPad;

}  // namespace SysMesBox
}  // namespace Darling
}  // namespace Profile
}  // namespace Impacto