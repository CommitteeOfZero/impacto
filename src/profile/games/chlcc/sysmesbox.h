#pragma once

#include "../../../hud/sysmesbox.h"
#include "../../../spritesheet.h"
#include "../../../games/chlcc/sysmesbox.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SysMesBox {

Impacto::SysMesBox::SysMesBoxBase* Configure();

extern Sprite Box;
extern Sprite BoxDecoration;

extern float BoxX;
extern float BoxY;

}  // namespace SysMesBox
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto