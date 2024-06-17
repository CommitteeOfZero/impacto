#pragma once

#include "../../../spritesheet.h"
#include "../../../games/chlcc/sysmesbox.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SysMesBox {

void Configure();

inline Sprite Box;
inline Sprite BoxDecoration;
inline Sprite SelectionLeftPart;
inline Sprite SelectionRightPart;
inline Sprite SelectionMiddlePart;

inline float BoxX;
inline float BoxY;
inline float ChoicePadding;
inline float ChoiceY;
inline float ChoiceXBase;
inline float MinMaxMesWidth;
inline float MinHighlightWidth;
inline float HighlightBaseWidth;
inline float HighlightRightPartSpriteWidth;
inline float HighlightYOffset;
inline float HighlightXOffset;
inline float HighlightXBase;
inline float HighlightXStep;

}  // namespace SysMesBox
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto