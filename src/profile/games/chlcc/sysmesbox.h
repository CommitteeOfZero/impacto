#pragma once

#include "../../../spritesheet.h"
#include "../../../games/chlcc/sysmesbox.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SysMesBox {

void Configure();

extern Sprite Box;
extern Sprite BoxDecoration;
extern Sprite SelectionLeftPart;
extern Sprite SelectionRightPart;
extern Sprite SelectionMiddlePart;

extern float BoxX;
extern float BoxY;
extern float ChoicePadding;
extern float ChoiceY;
extern float ChoiceXBase;
extern float MinMaxMesWidth;
extern float MinHighlightWidth;
extern float HighlightBaseWidth;
extern float HighlightRightPartSpriteWidth;
extern float HighlightYOffset;
extern float HighlightXOffset;
extern float HighlightXBase;
extern float HighlightXStep;

}  // namespace SysMesBox
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto