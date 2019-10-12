#pragma once

#include "../../../hud/sysmesbox.h"
#include "../../../spritesheet.h"
#include "../../../games/mo6tw/sysmesbox.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
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
extern float BoxMiddleRemainBase;
extern float BoxRightBaseX;
extern float BoxRightRemainPad;

}  // namespace SysMesBox
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto