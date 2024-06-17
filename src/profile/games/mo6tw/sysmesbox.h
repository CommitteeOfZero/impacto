#pragma once

#include "../../../spritesheet.h"
#include "../../../games/mo6tw/sysmesbox.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace SysMesBox {

void Configure();

inline Sprite BoxPartLeft;
inline Sprite BoxPartRight;
inline Sprite BoxPartMiddle;
inline Sprite BoxDecoration;
inline Sprite SelectionHighlight;

inline float BoxX;
inline float BoxY;
inline float ChoicePadding;
inline float ChoiceY;
inline float ChoiceXBase;
inline float MinMaxMesWidth;
inline float BoxMinimumWidth;
inline float BoxMiddleBaseX;
inline float BoxMiddleBaseWidth;
inline float BoxMiddleRemainBase;
inline float BoxRightBaseX;
inline float BoxRightRemainPad;

}  // namespace SysMesBox
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto